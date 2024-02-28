/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array simplex(Vec2<int>   shape,
              Vec2<float> kw,
              uint        seed,
              Array      *p_noise_x,
              Array      *p_noise_y,
              Array      *p_stretching,
              Vec2<float> shift,
              Vec2<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

  std::vector<float> x = linspace(0.5f * kw.x * shift.x,
                                  0.5f * kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(0.5f * kw.y * shift.y,
                                  0.5f * kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               [&noise](float x_, float y_, float)
                               { return noise.GetNoise(x_, y_); });
  return array;
}

} // namespace hmap
