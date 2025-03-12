/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <stdint.h>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/colormaps.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/range.hpp"
#include "highmap/shadows.hpp"

namespace hmap
{

void apply_hillshade(Tensor      &color3,
                     const Array &array,
                     float        vmin,
                     float        vmax,
                     float        exponent)
{
  // compute and scale hillshading
  Array hs = Array(array.shape, 1.f);
  hs = hillshade(array, 180.f, 45.f, 10.f * array.ptp() / (float)array.shape.y);
  remap(hs, vmin, vmax);

  if (exponent != 1.f) hs = pow(hs, exponent);

  clamp(hs);

  // apply to image
  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      for (int ch = 0; ch < 3; ch++)
        color3(i, j, ch) *= hs(i, j);
}

void apply_hillshade(std::vector<uint8_t> &img,
                     const Array          &array,
                     float                 vmin,
                     float                 vmax,
                     float                 exponent,
                     bool                  is_img_rgba)
{
  // compute and scale hillshading
  Array hs = Array(array.shape, 1.f);
  hs = hillshade(array, 180.f, 45.f, 10.f * array.ptp() / (float)array.shape.y);
  remap(hs, vmin, vmax);

  if (exponent != 1.f) hs = pow(hs, exponent);

  clamp(hs);

  // apply to image
  int k = 0;

  if (is_img_rgba)
  {
    for (int j = array.shape.y - 1; j > -1; j--)
      for (int i = 0; i < array.shape.x; i++)
      {
        img[k] = (uint8_t)((float)img[k] * hs(i, j));
        img[k + 1] = (uint8_t)((float)img[k + 1] * hs(i, j));
        img[k + 2] = (uint8_t)((float)img[k + 2] * hs(i, j));
        // skip alpha channel
        k += 4;
      }
  }
  else
  {
    for (int j = array.shape.y - 1; j > -1; j--)
      for (int i = 0; i < array.shape.x; i++)
      {
        img[k] = (uint8_t)((float)img[k] * hs(i, j));
        img[k + 1] = (uint8_t)((float)img[k + 1] * hs(i, j));
        img[k + 2] = (uint8_t)((float)img[k + 2] * hs(i, j));
        k += 3;
      }
  }
}

Tensor colorize(const Array &array,
                float        vmin,
                float        vmax,
                int          cmap,
                bool         hillshading,
                bool         reverse,
                const Array *p_noise)
{
  // get the colormap and reverse if needed
  const auto colormap_colors = get_colormap_data(cmap);
  if (reverse) std::swap(vmin, vmax);

  // initialize color tensor and normalization factors
  const int   nc = static_cast<int>(colormap_colors.size());
  Vec2<float> normalization_factors = array.normalization_coeff(vmin, vmax);
  normalization_factors.x *= (nc - 1);
  normalization_factors.y *= (nc - 1);

  Tensor color3(array.shape, 3);

  // lambda function to apply colormap
  auto apply_colormap = [&](float value) -> Vec3<float>
  {
    int   q = static_cast<int>(value);
    float t = value - q;
    if (q < nc - 1)
      return (1.f - t) * Vec3<float>(colormap_colors[q]) +
             t * Vec3<float>(colormap_colors[q + 1]);
    else
      return Vec3<float>(colormap_colors[q]);
  };

  // process each pixel
  for (int j = 0; j < array.shape.y; ++j)
    for (int i = 0; i < array.shape.x; ++i)
    {
      float value = array(i, j);
      if (p_noise) value += (*p_noise)(i, j);

      float vnorm = normalization_factors.x * value + normalization_factors.y;

      float normalized_value = std::clamp(vnorm,
                                          0.f,
                                          static_cast<float>(nc - 1));

      Vec3<float> color = apply_colormap(normalized_value);

      // assign color values to the tensor
      color3(i, j, 0) = color.x;
      color3(i, j, 1) = color.y;
      color3(i, j, 2) = color.z;
    }

  // apply hillshading if required
  if (hillshading) apply_hillshade(color3, array);

  return color3;
}

Tensor colorize_grayscale(const Array &array)
{
  Tensor color1 = Tensor(array.shape, 1);
  color1.set_slice(0, array);
  color1.remap();
  return color1;
}

Tensor colorize_histogram(const Array &array)
{
  Tensor color1 = Tensor(array.shape, 1);

  // normalization factors
  float a = 0.f;
  float b = 0.f;
  float vmin = array.min();
  float vmax = array.max();

  if (vmin != vmax)
  {
    a = 1.f / (vmax - vmin) * (float)(array.shape.x - 1);
    b = -vmin / (vmax - vmin) * (float)(array.shape.x - 1);
  }

  // compute histogram
  std::vector<int> hist(array.shape.x);
  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      hist[(int)(a * array(i, j) + b)] += 1;

  int hmax = *std::max_element(hist.begin(), hist.end());
  for (auto &v : hist)
    v = (int)((float)v / (float)hmax * (float)(array.shape.y - 1));

  // create histogram image
  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      if (j < hist[i]) color1(i, j, 0) = 1.f;

  return color1;
}

Tensor colorize_slope_height_heatmap(const Array &array, int cmap)
{
  Array dz = gradient_norm(array);

  // normalization factors / 1
  float a1 = 0.f;
  float b1 = 0.f;
  float vmin1 = array.min();
  float vmax1 = array.max();

  if (vmin1 != vmax1)
  {
    a1 = 1.f / (vmax1 - vmin1) * (float)(array.shape.x - 1);
    b1 = -vmin1 / (vmax1 - vmin1) * (float)(array.shape.x - 1);
  }

  // normalization factors / 2
  float a2 = 0.f;
  float b2 = 0.f;
  float vmin2 = dz.min();
  float vmax2 = dz.max();

  if (vmin2 != vmax2)
  {
    a2 = 1.f / (vmax2 - vmin2) * (float)(array.shape.y - 1);
    b2 = -vmin2 / (vmax2 - vmin2) * (float)(array.shape.y - 1);
  }

  // compute 2D histogram
  Array sum = Array(array.shape);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      int p = (int)(a1 * array(i, j) + b1);
      int q = (int)(a2 * dz(i, j) + b2);

      sum(p, q) += 1.f;
    }

  bool   hillshading = false;
  Tensor col3 = colorize(sum, sum.min(), sum.max(), cmap, hillshading);

  return col3;
}

Tensor colorize_vec2(const Array &array1, const Array &array2)
{
  // create image
  Tensor col3 = Tensor(array1.shape, 3);

  // normalization factors / 1
  float a1 = 0.f;
  float b1 = 0.f;
  float vmin1 = array1.min();
  float vmax1 = array1.max();

  if (vmin1 != vmax1)
  {
    a1 = 1.f / (vmax1 - vmin1);
    b1 = -vmin1 / (vmax1 - vmin1);
  }

  // normalization factors / 2
  float a2 = 0.f;
  float b2 = 0.f;
  float vmin2 = array2.min();
  float vmax2 = array2.max();

  if (vmin2 != vmax2)
  {
    a2 = 1.f / (vmax2 - vmin2);
    b2 = -vmin2 / (vmax2 - vmin2);
  }

  for (int j = 0; j < array1.shape.y; j++)
    for (int i = 0; i < array1.shape.x; i++)
    {
      float u = a1 * array1(i, j) + b1;
      float v = a2 * array2(i, j) + b2;
      float w = u * v * (1.f - u) * (1.f - v);

      col3(i, j, 0) = u;
      col3(i, j, 1) = v;
      col3(i, j, 2) = w;
    }

  return col3;
}

} // namespace hmap
