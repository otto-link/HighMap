/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/array.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
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

Array blend_poisson_bf(const Array &array1,
                       const Array &array2,
                       const int    iterations,
                       const Array *p_mask)
{
  Array array1_out = array1;

  auto run = clwrapper::Run("blend_poisson_bf");

  run.bind_buffer<float>("array1_out", array1_out.vector);
  run.bind_buffer<float>("array2", array2.vector);
  helper_bind_optional_buffer(run, "mask", p_mask);

  run.bind_arguments(array1.shape.x, array1.shape.y, p_mask ? 1 : 0);

  run.write_buffer("array1_out");
  run.write_buffer("array2");

  for (int it = 0; it < iterations; it++)
    run.execute({array1.shape.x, array1.shape.y});

  run.read_buffer("array1_out");

  return array1_out;
}

} // namespace hmap::gpu
