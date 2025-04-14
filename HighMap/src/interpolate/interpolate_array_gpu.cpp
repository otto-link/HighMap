/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

void interpolate_array_bicubic(const Array &source, Array &target)
{
  auto run = clwrapper::Run("interpolate_array_bicubic");

  run.bind_imagef("source", source.vector, source.shape.x, source.shape.y);
  run.bind_imagef("target",
                  target.vector,
                  target.shape.x,
                  target.shape.y,
                  true);

  run.bind_arguments(source.shape.x,
                     source.shape.y,
                     target.shape.x,
                     target.shape.y);

  run.execute({target.shape.x, target.shape.y});

  run.read_imagef("target");
}

void interpolate_array_bilinear(const Array &source, Array &target)
{
  auto run = clwrapper::Run("interpolate_array_bilinear");

  run.bind_imagef("source", source.vector, source.shape.x, source.shape.y);
  run.bind_imagef("target",
                  target.vector,
                  target.shape.x,
                  target.shape.y,
                  true);

  run.bind_arguments(target.shape.x, target.shape.y);

  run.execute({target.shape.x, target.shape.y});

  run.read_imagef("target");
}

void interpolate_array_lagrange(const Array &source, Array &target, int order)
{
  auto run = clwrapper::Run("interpolate_array_lagrange");

  run.bind_imagef("source", source.vector, source.shape.x, source.shape.y);
  run.bind_imagef("target",
                  target.vector,
                  target.shape.x,
                  target.shape.y,
                  true);

  run.bind_arguments(source.shape.x,
                     source.shape.y,
                     target.shape.x,
                     target.shape.y,
                     order);

  run.execute({target.shape.x, target.shape.y});

  run.read_imagef("target");
}

void interpolate_array_nearest(const Array &source, Array &target)
{
  auto run = clwrapper::Run("interpolate_array_nearest");

  run.bind_imagef("source", source.vector, source.shape.x, source.shape.y);
  run.bind_imagef("target",
                  target.vector,
                  target.shape.x,
                  target.shape.y,
                  true);

  run.bind_arguments(target.shape.x, target.shape.y);

  run.execute({target.shape.x, target.shape.y});

  run.read_imagef("target");
}

} // namespace hmap::gpu
