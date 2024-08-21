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
#include "highmap/math.hpp"
#include "highmap/range.hpp"
#include "highmap/shadows.hpp"

namespace hmap
{

void apply_hillshade(Array3            &color3,
                     const hmap::Array &array,
                     float              vmin,
                     float              vmax,
                     float              exponent)
{
  // compute and scale hillshading
  Array hs = Array(array.shape, 1.f);
  hs = hillshade(array, 180.f, 45.f, 10.f * array.ptp() / (float)array.shape.y);
  remap(hs, vmin, vmax);

  if (exponent != 1.f)
    hs = hmap::pow(hs, exponent);

  clamp(hs);

  // apply to image
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      for (int ch = 0; ch < 3; ch++)
        color3(i, j, ch) *= hs(i, j);
}

void apply_hillshade(std::vector<uint8_t> &img,
                     const hmap::Array    &array,
                     float                 vmin,
                     float                 vmax,
                     float                 exponent,
                     bool                  is_img_rgba)
{
  // compute and scale hillshading
  Array hs = Array(array.shape, 1.f);
  hs = hillshade(array, 180.f, 45.f, 10.f * array.ptp() / (float)array.shape.y);
  remap(hs, vmin, vmax);

  if (exponent != 1.f)
    hs = hmap::pow(hs, exponent);

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

Array3 colorize(Array &array,
                float  vmin,
                float  vmax,
                int    cmap,
                bool   hillshading,
                bool   reverse,
                Array *p_noise)
{
  std::vector<std::vector<float>> colormap_colors = get_colormap_data(cmap);

  if (reverse)
    std::swap(vmin, vmax);

  Array3 color3 = Array3(array.shape, 3);

  // normalization factors
  int         nc = (int)colormap_colors.size();
  Vec2<float> a = array.normalization_coeff(vmin, vmax);
  a.x *= (nc - 1);
  a.y *= (nc - 1);

  // colorize

  if (!p_noise)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float v = std::clamp(a.x * array(i, j) + a.y, 0.f, (float)nc - 1.f);
        int   q = (int)v;
        float t = v - q;

        if (q < nc - 1)
          for (int ch = 0; ch < 3; ch++)
            color3(i, j, ch) = (1.f - t) * colormap_colors[q][ch] +
                               t * colormap_colors[q + 1][ch];
        else
          for (int ch = 0; ch < 3; ch++)
            color3(i, j, ch) = colormap_colors[q][ch];
      }

  else
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float v = std::clamp(a.x * (array(i, j) + (*p_noise)(i, j)) + a.y,
                             0.f,
                             (float)nc - 1.f);
        int   q = (int)v;
        float t = v - q;

        if (q < nc - 1)
          for (int ch = 0; ch < 3; ch++)
            color3(i, j, ch) = (1.f - t) * colormap_colors[q][ch] +
                               t * colormap_colors[q + 1][ch];
        else
          for (int ch = 0; ch < 3; ch++)
            color3(i, j, ch) = colormap_colors[q][ch];
      }

  // add hillshading
  if (hillshading)
    apply_hillshade(color3, array);

  return color3;
}

Array3 colorize_grayscale(const Array &array)
{
  Array3 color1 = Array3(array.shape, 1);
  color1.remap();
  return color1;
}

Array3 colorize_histogram(const Array &array)
{
  Array3 color1 = Array3(array.shape, 1);

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
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      hist[(int)(a * array(i, j) + b)] += 1;

  int hmax = *std::max_element(hist.begin(), hist.end());
  for (auto &v : hist)
    v = (int)((float)v / (float)hmax * (float)(array.shape.y - 1));

  // create histogram image
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      if (j < hist[i])
        color1(i, j, 0) = 1.f;

  return color1;
}

Array3 colorize_vec2(const Array &array1, const Array &array2)
{
  // create image
  Array3 col3 = Array3(array1.shape, 3);

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

  for (int i = 0; i < array1.shape.x; i++)
    for (int j = 0; j < array1.shape.y; j++)
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