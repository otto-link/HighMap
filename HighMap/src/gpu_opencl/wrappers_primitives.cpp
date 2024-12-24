/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#ifdef ENABLE_OPENCL

#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

namespace hmap::gpu
{

Array noise(NoiseType   noise_type,
            Vec2<int>   shape,
            Vec2<float> kw,
            uint        seed,
            Array      *p_noise_x,
            Array      *p_noise_y,
            Array      *p_stretching,
            Vec4<float> bbox)
{
  Array array(shape);

  int noise_id = static_cast<int>(noise_type);
  LOG_DEBUG("noise_id: %d", noise_id);

  auto run = clwrapper::Run("noise");

  run.bind_buffer<float>("array", array.vector);
  helper_bind_optional_buffer(run, "noise_x", p_noise_x);
  helper_bind_optional_buffer(run, "noise_y", p_noise_y);

  run.bind_arguments(array.shape.x,
                     array.shape.y,
                     noise_id,
                     kw.x,
                     kw.y,
                     seed,
                     p_noise_x ? 1 : 0,
                     p_noise_y ? 1 : 0,
                     bbox);

  run.write_buffer("array");

  run.execute({array.shape.x, array.shape.y});

  run.read_buffer("array");

  array.infos();

  return array;
}

} // namespace hmap::gpu
#endif
