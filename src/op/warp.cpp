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

void warp(Array &array, const Array &dx, const Array &dy, float scale)
{
  int i1 = std::max(0, -(int)dx.min());
  int i2 = std::max(0, (int)dx.max());
  int j1 = std::max(0, -(int)dy.min());
  int j2 = std::max(0, (int)dy.max());

  Array array_buffered = generate_buffered_array(array, {i1, i2, j1, j2});

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      // warped position
      float x = (float)i + dx(i, j) * scale;
      float y = (float)j + dy(i, j) * scale;

      // nearest grid point (and bilinear interpolation parameters)
      int   ip = (int)x + i1;
      int   jp = (int)y + j1;
      float u = x - (float)(ip - i1);
      float v = y - (float)(jp - j1);

      array(i, j) = array_buffered.get_value_bilinear_at(ip, jp, u, v);
    }
}

void warp_fbm(Array             &array,
              float              scale,
              std::vector<float> kw,
              uint               seed,
              int                octaves,
              std::vector<float> shift)
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
  warp(array, dx, dy);
}

} // namespace hmap
