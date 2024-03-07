/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/noise_function.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array noise(NoiseType   noise_type,
            Vec2<int>   shape,
            Vec2<float> kw,
            uint        seed,
            Array      *p_noise_x,
            Array      *p_noise_y,
            Array      *p_stretching,
            Vec4<float> bbox)
{
  Array                          array = Array(shape);
  std::unique_ptr<NoiseFunction> p = create_noise_function_from_type(noise_type,
                                                                     kw,
                                                                     seed);

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               p.get()->get_function());
  return array;
}

} // namespace hmap
