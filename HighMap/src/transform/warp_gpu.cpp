/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

void warp(Array &array, const Array *p_dx, const Array *p_dy)
{
  if (p_dx && p_dy)
  {
    auto run = clwrapper::Run("warp_xy");
    run.bind_imagef("in", array.vector, array.shape.x, array.shape.y);
    run.bind_imagef("dx", p_dx->vector, p_dx->shape.x, p_dx->shape.y);
    run.bind_imagef("dy", p_dy->vector, p_dy->shape.x, p_dy->shape.y);
    run.bind_imagef("out",
                    array.vector,
                    array.shape.x,
                    array.shape.y,
                    true); // out
    run.bind_arguments(array.shape.x, array.shape.y);
    run.execute({array.shape.x, array.shape.y});
    run.read_imagef("out");
  }
  else if (p_dx)
  {
    auto run = clwrapper::Run("warp_x");
    run.bind_imagef("in", array.vector, array.shape.x, array.shape.y);
    run.bind_imagef("dx", p_dx->vector, p_dx->shape.x, p_dx->shape.y);
    run.bind_imagef("out",
                    array.vector,
                    array.shape.x,
                    array.shape.y,
                    true); // out
    run.bind_arguments(array.shape.x, array.shape.y);
    run.execute({array.shape.x, array.shape.y});
    run.read_imagef("out");
  }
  else if (p_dy)
  {
    auto run = clwrapper::Run("warp_y");
    run.bind_imagef("in", array.vector, array.shape.x, array.shape.y);
    run.bind_imagef("dy", p_dy->vector, p_dy->shape.x, p_dy->shape.y);
    run.bind_imagef("out",
                    array.vector,
                    array.shape.x,
                    array.shape.y,
                    true); // out
    run.bind_arguments(array.shape.x, array.shape.y);
    run.execute({array.shape.x, array.shape.y});
    run.read_imagef("out");
  }
}

} // namespace hmap::gpu
