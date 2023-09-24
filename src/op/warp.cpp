/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/kernels.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void warp(Array &array, const Array *p_dx, const Array *p_dy, float scale)
{
  int i1 = 1;
  int i2 = 1;
  int j1 = 1;
  int j2 = 1;

  std::function<void(float &, int &, int &)> lambda_x;
  std::function<void(float &, int &, int &)> lambda_y;

  if (p_dx)
  {
    i1 = std::max(1, -(int)(p_dx->min() * scale));
    i2 = std::max(1, (int)(p_dx->max() * scale));

    lambda_x = [p_dx, &scale](float &x_, int &i_, int &j_)
    { x_ = (float)i_ + (*p_dx)(i_, j_) * scale; };
  }
  else
    lambda_x = [](float &x_, int &i_, int &) { x_ = (float)i_; };

  if (p_dy)
  {
    j1 = std::max(1, -(int)(p_dy->min() * scale));
    j2 = std::max(1, (int)(p_dy->max() * scale));
    lambda_y = [p_dy, &scale](float &y_, int &i_, int &j_)
    { y_ = (float)j_ + (*p_dy)(i_, j_) * scale; };
  }
  else
    lambda_y = [](float &y_, int &, int &j_) { y_ = (float)j_; };

  Array array_buffered = generate_buffered_array(array, {i1, i2, j1, j2});

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      // warped position
      float x, y;
      lambda_x(x, i, j);
      lambda_y(y, i, j);

      // nearest grid point (and bilinear interpolation parameters)
      int   ip = (int)x + i1;
      int   jp = (int)y + j1;
      float u = x - (float)(ip - i1);
      float v = y - (float)(jp - j1);

      array(i, j) = array_buffered.get_value_bilinear_at(ip, jp, u, v);
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

    alpha = gradient_angle(array);
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

void warp_fbm(Array      &array,
              float       scale,
              Vec2<float> kw,
              uint        seed,
              int         octaves,
              Vec2<float> shift)
{
  float weight = 0.f;
  float persistence = 0.5f;
  float lacunarity = 2.f;

  Array dx = fbm_perlin(array.shape,
                        kw,
                        seed,
                        octaves,
                        weight,
                        persistence,
                        lacunarity,
                        nullptr,
                        nullptr,
                        shift);
  Array dy = fbm_perlin(array.shape,
                        kw,
                        seed++,
                        octaves,
                        weight,
                        persistence,
                        lacunarity,
                        nullptr,
                        nullptr,
                        shift);
  remap(dx, -scale, scale);
  remap(dy, -scale, scale);
  warp(array, &dx, &dy);
}

} // namespace hmap
