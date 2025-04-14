/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/interpolate2d.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

void interpolate_array_bicubic(const Array &source, Array &target)
{
  // use pixel-centered coordinates to be coherent with OpenCL image sampler
  float dx_s = 1.f / static_cast<float>(source.shape.x);
  float dy_s = 1.f / static_cast<float>(source.shape.y);

  float dx_t = 1.f / static_cast<float>(target.shape.x);
  float dy_t = 1.f / static_cast<float>(target.shape.y);

  for (int j = 0; j < target.shape.y; ++j)
    for (int i = 0; i < target.shape.x; ++i)
    {
      // pixel-coordinate
      float x = 0.5f * dx_t + static_cast<float>(i) * dx_t;
      float y = 0.5f * dy_t + static_cast<float>(j) * dy_t;

      // corresponding index for source array
      x = (x - 0.5f * dx_s) / dx_s;
      y = (y - 0.5f * dy_s) / dy_s;

      int is0 = static_cast<int>(x);
      int js0 = static_cast<int>(y);

      float u = x - is0;
      float v = y - js0;

      // interpolate
      float arr[4][4];

      // get the 4x4 surrounding grid points
      for (int n = -1; n <= 2; ++n)
        for (int m = -1; m <= 2; ++m)
        {
          int ip = std::clamp(is0 + m, 0, source.shape.x - 1);
          int jp = std::clamp(js0 + n, 0, source.shape.y - 1);
          arr[m + 1][n + 1] = source(ip, jp);
        }

      // interpolate in the x direction
      float col_results[4];
      for (int k = 0; k < 4; ++k)
        col_results[k] = cubic_interpolate(arr[k], v);

      // interpolate in the y direction
      target(i, j) = cubic_interpolate(col_results, u);
    }
}

void interpolate_array_bilinear(const Array &source, Array &target)
{
  // use pixel-centered coordinates to be coherent with OpenCL image sampler
  float dx_s = 1.f / static_cast<float>(source.shape.x);
  float dy_s = 1.f / static_cast<float>(source.shape.y);

  float dx_t = 1.f / static_cast<float>(target.shape.x);
  float dy_t = 1.f / static_cast<float>(target.shape.y);

  for (int j = 0; j < target.shape.y; ++j)
    for (int i = 0; i < target.shape.x; ++i)
    {
      // pixel-coordinate
      float x = 0.5f * dx_t + static_cast<float>(i) * dx_t;
      float y = 0.5f * dy_t + static_cast<float>(j) * dy_t;

      // corresponding index for source array
      x = (x - 0.5f * dx_s) / dx_s;
      y = (y - 0.5f * dy_s) / dy_s;

      int is0 = static_cast<int>(x);
      int js0 = static_cast<int>(y);

      float u = x - is0;
      float v = y - js0;

      int is1 = std::min(is0 + 1, source.shape.x - 1);
      int js1 = std::min(js0 + 1, source.shape.y - 1);

      target(i, j) = bilinear_interp(source(is0, js0),
                                     source(is1, js0),
                                     source(is0, js1),
                                     source(is1, js1),
                                     u,
                                     v);
    }
}

void interpolate_array_nearest(const Array &source, Array &target)
{
  // coefficients to switch between target indices to source indices
  float coeff_x = static_cast<float>(source.shape.x) /
                  static_cast<float>(target.shape.x);
  float coeff_y = static_cast<float>(source.shape.y) /
                  static_cast<float>(target.shape.y);

  for (int j = 0; j < target.shape.y; ++j)
    for (int i = 0; i < target.shape.x; ++i)
    {
      int is = static_cast<int>(coeff_x * i);
      int js = static_cast<int>(coeff_y * j);

      target(i, j) = source(is, js);
    }
}

void interpolate_array_nearest(const Array       &source,
                               Array             &target,
                               const Vec4<float> &bbox_source,
                               const Vec4<float> &bbox_target)
{
  bool endpoint = false;

  std::vector<float> x = linspace(bbox_target.a,
                                  bbox_target.b,
                                  target.shape.x,
                                  endpoint);

  std::vector<float> y = linspace(bbox_target.c,
                                  bbox_target.d,
                                  target.shape.y,
                                  endpoint);

  for (int j = 0; j < target.shape.y; ++j)
    for (int i = 0; i < target.shape.x; ++i)
    {
      int is = static_cast<int>((x[i] - bbox_source.a) /
                                (bbox_source.b - bbox_source.a) *
                                source.shape.x);
      int js = static_cast<int>((y[j] - bbox_source.c) /
                                (bbox_source.d - bbox_source.c) *
                                source.shape.y);

      is = std::clamp(is, 0, source.shape.x - 1);
      js = std::clamp(js, 0, source.shape.y - 1);

      target(i, j) = source(is, js);
    }
}

} // namespace hmap
