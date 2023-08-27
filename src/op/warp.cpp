/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void warp(Array &array, const Array *p_dx, const Array *p_dy, float scale)
{
  int i1 = 0;
  int i2 = 0;
  int j1 = 0;
  int j2 = 0;

  std::function<void(float &, int &, int &)> lambda_x;
  std::function<void(float &, int &, int &)> lambda_y;

  if (p_dx)
  {
    i1 = std::max(0, -(int)p_dx->min());
    i2 = std::max(0, (int)p_dx->max());

    lambda_x = [p_dx, &scale](float &x_, int &i_, int &j_)
    { x_ = (float)i_ + (*p_dx)(i_, j_) * scale; };
  }
  else
    lambda_x = [](float &x_, int &i_, int &) { x_ = (float)i_; };

  if (p_dy)
  {
    j1 = std::max(0, -(int)p_dy->min());
    j2 = std::max(0, (int)p_dy->max());
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
