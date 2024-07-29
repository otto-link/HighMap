/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/array.hpp"
#include "highmap/functions.hpp"
#include "highmap/geometry.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"
#include "highmap/vector.hpp"

namespace hmap
{

Array slope(Vec2<int>   shape,
            float       angle,
            float       slope,
            Array      *p_noise_x,
            Array      *p_noise_y,
            Array      *p_stretching,
            Vec2<float> center,
            Vec4<float> bbox)
{
  Array               array = Array(shape);
  hmap::SlopeFunction f = hmap::SlopeFunction(angle, slope, center);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

} // namespace hmap
