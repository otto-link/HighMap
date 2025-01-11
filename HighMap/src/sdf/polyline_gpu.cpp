/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry/grids.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

Array sdf_2d_polyline(const Path &path,
                      Vec2<int>   shape,
                      Vec4<float> bbox_path,
                      Array      *p_noise_x,
                      Array      *p_noise_y,
                      Vec4<float> bbox_array)
{
  Array sdf2(shape);

  // normalize path nodes coord to unit square
  std::vector<float> xp = path.get_x();
  std::vector<float> yp = path.get_y();
  rescale_grid_to_unit_square(xp, yp, bbox_path);

  // kernel
  auto run = clwrapper::Run("sdf_2d_polyline");

  run.bind_buffer<float>("sdf2", sdf2.vector);
  helper_bind_optional_buffer(run, "noise_x", p_noise_x);
  helper_bind_optional_buffer(run, "noise_y", p_noise_y);
  run.bind_buffer<float>("xp", xp);
  run.bind_buffer<float>("yp", yp);

  run.bind_arguments(shape.x,
                     shape.y,
                     (int)xp.size(),
                     p_noise_x ? 1 : 0,
                     p_noise_y ? 1 : 0,
                     bbox_array);

  run.write_buffer("xp");
  run.write_buffer("yp");

  run.execute({shape.x, shape.y});

  run.read_buffer("sdf2");

  return sdf2;
}

} // namespace hmap::gpu
