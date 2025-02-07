/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

Array blend_gradients(const Array &array1, const Array &array2, int ir)
{
  Array dn1 = gpu::gradient_norm(array1);
  Array dn2 = gpu::gradient_norm(array2);

  gpu::smooth_cpulse(dn1, ir);
  gpu::smooth_cpulse(dn2, ir);

  Array t = gpu::maximum_smooth(dn1, dn2, 0.1f / (float)array1.shape.x);
  remap(t);

  return lerp(array1, array2, t);
}

} // namespace hmap::gpu
