/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

Array maximum_smooth(const Array &array1, const Array &array2, float k)
{
  Array array_out = array1;

  auto run = clwrapper::Run("maximum_smooth");

  run.bind_buffer("array1", array_out.vector);
  run.bind_buffer("array2", const_cast<std::vector<float> &>(array2.vector));
  run.bind_arguments(array1.shape.x, array1.shape.y, k);

  run.write_buffer("array1");
  run.write_buffer("array2");

  run.execute({array1.shape.x, array1.shape.y});

  run.read_buffer("array1");

  return array_out;
}

Array minimum_smooth(const Array &array1, const Array &array2, float k)
{
  Array array_out = array1;

  auto run = clwrapper::Run("minimum_smooth");

  run.bind_buffer("array1", array_out.vector);
  run.bind_buffer("array2", const_cast<std::vector<float> &>(array2.vector));
  run.bind_arguments(array1.shape.x, array1.shape.y, k);

  run.write_buffer("array1");
  run.write_buffer("array2");

  run.execute({array1.shape.x, array1.shape.y});

  run.read_buffer("array1");

  return array_out;
}

} // namespace hmap::gpu
