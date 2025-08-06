/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

void rotate(Array &array, float angle, bool zoom_in)
{
  auto run = clwrapper::Run("rotate");
  run.bind_imagef("in", array.vector, array.shape.x, array.shape.y);
  run.bind_imagef("out",
                  array.vector,
                  array.shape.x,
                  array.shape.y,
                  true); // out
  run.bind_arguments(array.shape.x, array.shape.y, angle, zoom_in ? 1 : 0);
  run.execute({array.shape.x, array.shape.y});
  run.read_imagef("out");
}

} // namespace hmap::gpu
