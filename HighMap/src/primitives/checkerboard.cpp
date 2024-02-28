/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array checkerboard(Vec2<int>   shape,
                   Vec2<float> kw,
                   Array      *p_noise_x,
                   Array      *p_noise_y,
                   Vec2<float> shift,
                   Vec2<float> scale)
{
  Array array = Array(shape);

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  auto lambda = [&kw](float x_, float y_, float)
  {
    return std::abs(std::abs((int)std::floor(x_) % 2) -
                    std::abs((int)std::floor(y_) % 2));
  };

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               nullptr,
                               lambda);
  return array;
}

} // namespace hmap
