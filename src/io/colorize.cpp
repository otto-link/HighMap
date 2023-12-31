/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <stdint.h>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/colormaps.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void apply_hillshade(std::vector<uint8_t> &img, const hmap::Array &array)
{
  Array hs = constant(array.shape, 1.f);
  hs = hillshade(array, 180.f, 45.f, 10.f * array.ptp() / (float)array.shape.y);
  remap(hs);

  int k = 0;
  for (int j = array.shape.y - 1; j > -1; j--)
    for (int i = 0; i < array.shape.x; i++)
    {
      img[k] = (uint8_t)((float)img[k] * hs(i, j));
      img[k + 1] = (uint8_t)((float)img[k + 1] * hs(i, j));
      img[k + 2] = (uint8_t)((float)img[k + 2] * hs(i, j));
      k += 3;
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

// std::vector<uint8_t> colorize_trivariate(const Array &c0,
//                                          const Array &c1,
//                                          const Array &c2,
//                                          Clut3D      &clut,
//                                          bool         hillshading)
// {
//   std::vector<uint8_t> img(3 * c0.shape.x * c0.shape.y);

//   // TODO vlim and clamping

//   // normalization coefficients for each array
//   float cmin = c0.min();
//   float cmax = c0.max();
//   float ac0 = 1.f / (cmax - cmin);
//   float bc0 = -cmin / (cmax - cmin);

//   cmin = c1.min();
//   cmax = c1.max();
//   float ac1 = 1.f / (cmax - cmin);
//   float bc1 = -cmin / (cmax - cmin);

//   cmin = c2.min();
//   cmax = c2.max();
//   float ac2 = 1.f / (cmax - cmin);
//   float bc2 = -cmin / (cmax - cmin);

//   int k = 0;

//   for (int j = c0.shape.y - 1; j > -1; j--)
//     for (int i = 0; i < c0.shape.x; i++)
//     {
//       // linear interpolation for the first criterion (generally
//       // dominant)
//       float u;
//       int   p1 = clut.get_index(ac0 * c0(i, j) + bc0, 0, u);
//       int   p2 = clut.get_index(ac1 * c1(i, j) + bc1, 1);
//       int   p3 = clut.get_index(ac2 * c2(i, j) + bc2, 2);

//       UNPACK_COLOR(clut.value_at(p1, p2, p3),
//                    uint8_t r0,
//                    uint8_t g0,
//                    uint8_t b0);

//       if (p1 < clut.shape[0] - 1)
//       {
//         UNPACK_COLOR(clut.value_at(p1 + 1, p2, p3),
//                      uint8_t r1,
//                      uint8_t g1,
//                      uint8_t b1);

//         img[k++] = (1.f - u) * r0 + u * r1;
//         img[k++] = (1.f - u) * g0 + u * g1;
//         img[k++] = (1.f - u) * b0 + u * b1;
//       }
//       else
//       {
//         img[k++] = r0;
//         img[k++] = g0;
//         img[k++] = b0;
//       }
//     }

//   // blur colors to smooth nearest interpolation
//   {
//     std::vector<uint8_t> delta(3 * c0.shape.x * c0.shape.y);

//     for (int i = 1; i < c0.shape.x - 1; i++)
//       for (int j = 1; j < c0.shape.y - 1; j++)
//       {
//         int k = j * 3 + i * 3 * c0.shape.y;
//         int kn = (j + 1) * 3 + i * 3 * c0.shape.y; // north
//         int ks = (j - 1) * 3 + i * 3 * c0.shape.y; // south
//         int ke = j * 3 + (i - 1) * 3 * c0.shape.y; // east
//         int kw = j * 3 + (i + 1) * 3 * c0.shape.y; // west

//         for (int r = 0; r < 3; r++)
//           delta[k + r] = (uint8_t)(0.25f * (img[kn + r] + img[ks + r] +
//                                             img[ke + r] + img[kw + r])) -
//                          img[k + r];
//       }

//     std::transform(img.begin(),
//                    img.end(),
//                    delta.begin(),
//                    img.begin(),
//                    [](uint8_t i, uint8_t d) { return i + d; });
//   }

//   // add hillshading
//   if (hillshading)
//     apply_hillshade(img, c0);

//   return img;
// }

} // namespace hmap
