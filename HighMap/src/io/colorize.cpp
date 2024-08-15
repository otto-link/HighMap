/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <stdint.h>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/colormaps.hpp"
#include "highmap/io.hpp"
#include "highmap/math.hpp"
#include "highmap/range.hpp"
#include "highmap/shadows.hpp"

namespace hmap
{

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

std::vector<uint8_t> colorize(hmap::Array &array,
                              float        vmin,
                              float        vmax,
                              int          cmap,
                              bool         hillshading,
                              bool         reverse)
{
  std::vector<std::vector<float>> colors = get_colormap_data(cmap);

  if (reverse)
    std::swap(vmin, vmax);

  // create image
  std::vector<uint8_t> img(3 * array.shape.x * array.shape.y);

  // normalization factors
  int         nc = (int)colors.size();
  Vec2<float> a = array.normalization_coeff(vmin, vmax);
  a.x *= (nc - 1);
  a.y *= (nc - 1);

  // reorganize things to get an image with (i, j) used as (x, y)
  // coordinates, i.e. with (0, 0) at the bottom left
  int k = 0;

  for (int j = array.shape.y - 1; j > -1; j--)
    for (int i = 0; i < array.shape.x; i++)
    {
      float v = std::clamp(a.x * array(i, j) + a.y, 0.f, (float)nc - 1.f);
      int   q = (int)v;
      float t = v - q;

      if (q < nc - 1)
        for (int ch = 0; ch < 3; ch++)
          img[k++] = (uint8_t)(255.f * ((1.f - t) * colors[q][ch] +
                                        t * colors[q + 1][ch]));
      else
        for (int ch = 0; ch < 3; ch++)
          img[k++] = (uint8_t)(255.f * colors[q][ch]);
    }

  // add hillshading
  if (hillshading)
    apply_hillshade(img, array);

  return img;
}

std::vector<uint8_t> colorize_grayscale(const Array &array, Vec2<int> step)
{
  // create image
  std::vector<uint8_t> img(array.shape.x * array.shape.y / step.x / step.y);

  // normalization factors
  float a = 0.f;
  float b = 0.f;
  float vmin = array.min();
  float vmax = array.max();

  if (vmin != vmax)
  {
    a = 1.f / (vmax - vmin);
    b = -vmin / (vmax - vmin);
  }

  int k = 0;

  for (int j = array.shape.y - 1; j > -1; j -= step.y)
    for (int i = 0; i < array.shape.x; i += step.x)
    {
      float v = a * array(i, j) + b;
      img[k++] = (uint8_t)(v * 255.f);
    }

  return img;
}

std::vector<uint8_t> colorize_histogram(const Array &array, Vec2<int> step)
{
  // create image
  std::vector<uint8_t> img(array.shape.x * array.shape.y / step.x / step.y);

  // normalization factors
  float a = 0.f;
  float b = 0.f;
  float vmin = array.min();
  float vmax = array.max();

  if (vmin != vmax)
  {
    a = 1.f / (vmax - vmin) * (float)(array.shape.x / step.x - 1);
    b = -vmin / (vmax - vmin) * (float)(array.shape.x / step.x - 1);
  }

  // compute histogram
  std::vector<int> hist(array.shape.x / step.x);
  for (int i = 0; i < array.shape.x; i += step.x)
    for (int j = 0; j < array.shape.y; j += step.y)
      hist[(int)(a * array(i, j) + b)] += 1;

  int hmax = *std::max_element(hist.begin(), hist.end());
  for (auto &v : hist)
    v = (int)((float)v / (float)hmax * (float)(array.shape.y / step.y - 1));

  // create histogram image
  int k = 0;

  for (int j = array.shape.y / step.y - 1; j > -1; j--)
    for (int i = 0; i < array.shape.x / step.x; i++)
    {
      if (j < hist[i])
        img[k++] = 255;
      else
        img[k++] = 0;
    }

  return img;
}

std::vector<uint8_t> colorize_vec2(const Array &array1, const Array &array2)
{
  // create image
  std::vector<uint8_t> img(3 * array1.shape.x * array1.shape.y);

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

  int k = 0;

  for (int j = array1.shape.y - 1; j > -1; j--)
    for (int i = 0; i < array1.shape.x; i++)
    {
      float u = a1 * array1(i, j) + b1;
      float v = a2 * array2(i, j) + b2;
      float w = u * v * (1.f - u) * (1.f - v);

      img[k++] = (uint8_t)(255.f * u);
      img[k++] = (uint8_t)(255.f * v);
      img[k++] = (uint8_t)(255.f * w);
    }

  return img;
}

} // namespace hmap
