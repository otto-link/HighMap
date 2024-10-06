/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

void upscale_amplification(Array                        &array,
                           int                           upscaling_levels,
                           std::function<void(Array &x)> unary_op)
{
  Vec2<int> initial_shape = array.shape;

  // upscale amplification (NB, k = 0 corresponds to initial resolution)
  for (int k = 0; k < upscaling_levels + 1; k++)
  {
    Vec2<int> upscaled_shape = std::pow(2, k) * initial_shape;

    array = array.resample_to_shape_bicubic(upscaled_shape);
    unary_op(array);
  }

  // go back to original resolution (bilinear interpolation)
  array = array.resample_to_shape(initial_shape);
}

} // namespace hmap
