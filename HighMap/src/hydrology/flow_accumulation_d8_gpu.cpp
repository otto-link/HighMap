/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/hydrology.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

Array flow_accumulation_d8(const Array &z)
{
  // TODO not functional

  Mat<int>    facc = Mat<int>(z.shape);
  const Array d8 = gpu::flow_direction_d8(z);

  for (int j = 0; j < z.shape.y; j++)
    for (int i = 0; i < z.shape.x; i++)
      facc(i, j) = 1;

  auto run = clwrapper::Run("flow_accumulation_d8");
  run.bind_buffer<float>("z", const_cast<std::vector<float> &>(z.vector));
  run.bind_buffer<float>("d8", const_cast<std::vector<float> &>(d8.vector));
  run.bind_buffer<int>("facc", facc.vector);
  run.bind_arguments(z.shape.x, z.shape.y);

  run.write_buffer("z");
  run.write_buffer("d8");
  run.write_buffer("facc");

  for (int it = 0; it < 200; it++)
    run.execute({z.shape.x, z.shape.y});

  run.read_buffer("facc");

  // convert output
  Array facc_out(z.shape);

  for (int j = 0; j < z.shape.y; j++)
    for (int i = 0; i < z.shape.x; i++)
      facc_out(i, j) = static_cast<float>(facc(i, j));

  facc_out.infos();

  return facc_out;
}

Array flow_direction_d8(const Array &z)
{
  Array d8 = Array(z.shape);

  auto run = clwrapper::Run("flow_direction_d8");
  run.bind_buffer<float>("z", const_cast<std::vector<float> &>(z.vector));
  run.bind_buffer<float>("d8", d8.vector);
  run.bind_arguments(z.shape.x, z.shape.y);

  run.write_buffer("z");
  run.execute({z.shape.x, z.shape.y});
  run.read_buffer("d8");

  return d8;
}

} // namespace hmap::gpu
