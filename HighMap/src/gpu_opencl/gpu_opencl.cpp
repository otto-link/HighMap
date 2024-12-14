/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#ifdef ENABLE_OPENCL

#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/transform.hpp"

namespace hmap::gpu
{

bool init_opencl()
{
  if (!clwrapper::DeviceManager::get_instance().is_ready()) return false;

  // load and build kernels
  const std::string code =
#include "kernels/median_3x3.cl"
      ;

  clwrapper::KernelManager::get_instance().add_kernel(code);

  return true;
}

void median_3x3(Array &array)
{
  Vec2<int> shape = array.shape;

  auto run = clwrapper::Run("median_3x3");

  run.bind_imagef("in", array.vector, shape.x, shape.y);
  run.bind_imagef("out", array.vector, shape.x, shape.y, true); // out
  run.bind_arguments(shape.x, shape.y);

  run.execute({shape.x, shape.y});

  run.read_imagef("out");
}

void avg(Array &array, int ir)
{
  Vec2<int> shape = array.shape;

  auto run = clwrapper::Run("avg");

  // FIX ME transpose / row-col major compatibility issue

  transpose(array);

  run.bind_imagef("in", array.vector, shape.y, shape.x);
  run.bind_imagef("out", array.vector, shape.y, shape.x, true); // out
  run.bind_arguments(shape.y, shape.x, ir);

  run.execute({shape.y, shape.x});

  run.read_imagef("out");

  transpose(array);
}

} // namespace hmap::gpu
#endif
