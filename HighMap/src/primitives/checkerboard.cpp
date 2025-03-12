/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/array.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array checkerboard(Vec2<int>    shape,
                   Vec2<float>  kw,
                   const Array *p_noise_x,
                   const Array *p_noise_y,
                   const Array *p_stretching,
                   Vec4<float>  bbox)
{
  Array array = Array(shape);

  auto lambda = [&kw](float x, float y, float)
  {
    return std::abs(std::abs((int)std::floor(x) % 2) -
                    std::abs((int)std::floor(y) % 2));
  };

  fill_array_using_xy_function(array,
                               bbox,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               lambda);
  return array;
}

} // namespace hmap
