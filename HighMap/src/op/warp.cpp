/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/kernels.hpp"
#include "highmap/noise_function.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void warp(Array &array, Array *p_dx, Array *p_dy)
{
  hmap::ArrayFunction f = hmap::ArrayFunction(array,
                                              Vec2<float>(1.f, 1.f),
                                              true);

  fill_array_using_xy_function(array,
                               {0.f, 1.f, 0.f, 1.f},
                               p_dx,
                               p_dy,
                               nullptr,
                               f.get_function());
}

void warp_directional(Array &array,
                      float  angle,
                      float  amount,
                      int    ir,
                      bool   reverse)
{
  // same as warp_downslope but add a reference angle to scale the
  // amount of warping
  float arad = angle / 180.f * M_PI;

  Array array_new = Array(array.shape);
  Array alpha = Array(array.shape);

  {
    Array array_f = array;
    if (ir > 0)
      smooth_cpulse(array_f, ir);

    alpha = gradient_angle(array_f);
  }

  // add a shape factor to avoid artifacts close to the boundaries
  Array factor = smooth_cosine(array.shape);

  if (reverse)
    amount = -amount;

  for (int i = 1; i < array.shape.x - 1; i++)
    for (int j = 1; j < array.shape.y - 1; j++)
    {

      float x = (float)i + amount * std::cos(alpha(i, j) - arad) * factor(i, j);
      float y = (float)j + amount * std::sin(alpha(i, j) - arad) * factor(i, j);

      // bilinear interpolation parameters
      int ip = std::clamp((int)x, 0, array.shape.x - 1);
      int jp = std::clamp((int)y, 0, array.shape.y - 1);

      float u = std::clamp(x - (float)ip, 0.f, 1.f);
      float v = std::clamp(y - (float)jp, 0.f, 1.f);

      array_new(i, j) = array.get_value_bilinear_at(ip, jp, u, v);
    }
  extrapolate_borders(array_new);

  array = array_new;
}

void warp_directional(Array &array,
                      float  angle,
                      Array *p_mask,
                      float  amount,
                      int    ir,
                      bool   reverse)
{
  if (!p_mask)
    warp_directional(array, angle, amount, ir, reverse);
  else
  {
    Array array_f = array;
    warp_directional(array_f, angle, amount, ir, reverse);
    array = lerp(array, array_f, *(p_mask));
  }
}

void warp_downslope(Array &array, float amount, int ir, bool reverse)
{
  Array array_new = Array(array.shape);
  Array alpha = Array(array.shape);

  {
    Array array_f = array;
    if (ir > 0)
      smooth_cpulse(array_f, ir);

    alpha = gradient_angle(array_f);
  }

  // add a shape factor to avoid artifacts close to the boundaries
  Array factor = smooth_cosine(array.shape);

  if (reverse)
    amount = -amount;

  for (int i = 1; i < array.shape.x - 1; i++)
    for (int j = 1; j < array.shape.y - 1; j++)
    {

      float x = (float)i + amount * std::cos(alpha(i, j)) * factor(i, j);
      float y = (float)j + amount * std::sin(alpha(i, j)) * factor(i, j);

      // bilinear interpolation parameters
      int ip = std::clamp((int)x, 0, array.shape.x - 1);
      int jp = std::clamp((int)y, 0, array.shape.y - 1);

      float u = std::clamp(x - (float)ip, 0.f, 1.f);
      float v = std::clamp(y - (float)jp, 0.f, 1.f);

      array_new(i, j) = array.get_value_bilinear_at(ip, jp, u, v);
    }
  extrapolate_borders(array_new);

  array = array_new;
}

void warp_downslope(Array &array,
                    Array *p_mask,
                    float  amount,
                    int    ir,
                    bool   reverse)
{
  if (!p_mask)
    warp_downslope(array, amount, ir, reverse);
  else
  {
    Array array_f = array;
    warp_downslope(array_f, amount, ir, reverse);
    array = lerp(array, array_f, *(p_mask));
  }
}

} // namespace hmap
