/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

Array gradient_norm(const Array &array)
{
  Array dm(array.shape);

  auto run = clwrapper::Run("gradient_norm");

  run.bind_buffer<float>("array",
                         const_cast<std::vector<float> &>(array.vector));
  run.bind_buffer<float>("dm", dm.vector);
  run.bind_arguments(array.shape.x, array.shape.y);

  run.write_buffer("array");

  run.execute({array.shape.x, array.shape.y});

  run.read_buffer("dm");

  return dm;
}

} // namespace hmap::gpu
