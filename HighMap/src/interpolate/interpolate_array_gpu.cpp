/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

// helpers

Vec4<float> helper_transform_bbox(const Vec4<float> &bbox_source,
                                  const Vec4<float> &bbox_target)
{
  // in the OpenCL kernel, the bounding box of the source array is
  // assumed to be a unit square. Shift and rescale the target
  // bounding box according to this hypothesis
  Vec4<float> bbox_target_mod = bbox_target;

  bbox_target_mod.a = (bbox_target_mod.a - bbox_source.a) /
                      (bbox_source.b - bbox_source.a);
  bbox_target_mod.b = (bbox_target_mod.b - bbox_source.a) /
                      (bbox_source.b - bbox_source.a);

  bbox_target_mod.c = (bbox_target_mod.c - bbox_source.c) /
                      (bbox_source.d - bbox_source.c);
  bbox_target_mod.d = (bbox_target_mod.d - bbox_source.c) /
                      (bbox_source.d - bbox_source.c);

  return bbox_target_mod;
}

// functions

void interpolate_array_bicubic(const Array &source, Array &target)
{
  Vec4<float> bbox(0.f, 1.f, 0.f, 1.f);

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
                     target.shape.y,
                     bbox);

  run.execute({target.shape.x, target.shape.y});

  run.read_imagef("target");
}

void interpolate_array_bicubic(const Array       &source,
                               Array             &target,
                               const Vec4<float> &bbox_source,
                               const Vec4<float> &bbox_target)
{
  Vec4<float> bbox_target_mod = helper_transform_bbox(bbox_source, bbox_target);

  // compute
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
                     target.shape.y,
                     bbox_target_mod);

  run.execute({target.shape.x, target.shape.y});

  run.read_imagef("target");
}

void interpolate_array_bilinear(const Array &source, Array &target)
{
  Vec4<float> bbox(0.f, 1.f, 0.f, 1.f);

  auto run = clwrapper::Run("interpolate_array_bilinear");

  run.bind_imagef("source", source.vector, source.shape.x, source.shape.y);
  run.bind_imagef("target",
                  target.vector,
                  target.shape.x,
                  target.shape.y,
                  true);

  run.bind_arguments(target.shape.x, target.shape.y, bbox);

  run.execute({target.shape.x, target.shape.y});

  run.read_imagef("target");
}

void interpolate_array_bilinear(const Array       &source,
                                Array             &target,
                                const Vec4<float> &bbox_source,
                                const Vec4<float> &bbox_target)
{
  Vec4<float> bbox_target_mod = helper_transform_bbox(bbox_source, bbox_target);

  // compute
  auto run = clwrapper::Run("interpolate_array_bilinear");

  run.bind_imagef("source", source.vector, source.shape.x, source.shape.y);
  run.bind_imagef("target",
                  target.vector,
                  target.shape.x,
                  target.shape.y,
                  true);

  run.bind_arguments(target.shape.x, target.shape.y, bbox_target_mod);

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
  Vec4<float> bbox(0.f, 1.f, 0.f, 1.f);

  auto run = clwrapper::Run("interpolate_array_nearest");

  run.bind_imagef("source", source.vector, source.shape.x, source.shape.y);
  run.bind_imagef("target",
                  target.vector,
                  target.shape.x,
                  target.shape.y,
                  true);

  run.bind_arguments(target.shape.x, target.shape.y, bbox);

  run.execute({target.shape.x, target.shape.y});

  run.read_imagef("target");
}

void interpolate_array_nearest(const Array       &source,
                               Array             &target,
                               const Vec4<float> &bbox_source,
                               const Vec4<float> &bbox_target)
{
  Vec4<float> bbox_target_mod = helper_transform_bbox(bbox_source, bbox_target);

  // compute
  auto run = clwrapper::Run("interpolate_array_nearest");

  run.bind_imagef("source", source.vector, source.shape.x, source.shape.y);
  run.bind_imagef("target",
                  target.vector,
                  target.shape.x,
                  target.shape.y,
                  true);

  run.bind_arguments(target.shape.x, target.shape.y, bbox_target_mod);

  run.execute({target.shape.x, target.shape.y});

  run.read_imagef("target");
}

} // namespace hmap::gpu
