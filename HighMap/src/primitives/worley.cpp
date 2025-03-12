/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/functions.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array worley_double(Vec2<int>    shape,
                    Vec2<float>  kw,
                    uint         seed,
                    float        ratio,
                    float        k,
                    const Array *p_ctrl_param,
                    const Array *p_noise_x,
                    const Array *p_noise_y,
                    const Array *p_stretching,
                    Vec4<float>  bbox)
{
  hmap::Array                array = hmap::Array(shape);
  hmap::WorleyDoubleFunction f = hmap::WorleyDoubleFunction(kw, seed, ratio, k);

  fill_array_using_xy_function(array,
                               bbox,
                               p_ctrl_param,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

} // namespace hmap
