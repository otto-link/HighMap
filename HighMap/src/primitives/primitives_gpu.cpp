/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#ifdef ENABLE_OPENCL

#include "highmap/array.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

Array gabor_wave(Vec2<int> shape, Vec2<float> kw, uint seed, Vec4<float> bbox)
{
  Array array(shape);

  auto run = clwrapper::Run("gabor_wave");

  run.bind_buffer<float>("array", array.vector);
  run.bind_arguments(array.shape.x, array.shape.y, kw.x, kw.y, seed, bbox);

  run.write_buffer("array");
  run.execute({array.shape.x, array.shape.y});
  run.read_buffer("array");

  return array;
}

Array gabor_wave_fbm(Vec2<int>   shape,
                     Vec2<float> kw,
                     uint        seed,
                     int         octaves,
                     float       weight,
                     float       persistence,
                     float       lacunarity,
                     Vec4<float> bbox)
{
  Array array(shape);

  auto run = clwrapper::Run("gabor_wave_fbm");

  run.bind_buffer<float>("array", array.vector);
  run.bind_arguments(array.shape.x,
                     array.shape.y,
                     kw.x,
                     kw.y,
                     seed,
                     octaves,
                     weight,
                     persistence,
                     lacunarity,
                     bbox);

  run.write_buffer("array");
  run.execute({array.shape.x, array.shape.y});
  run.read_buffer("array");

  return array;
}

Array voronoise(Vec2<int>   shape,
                Vec2<float> kw,
                float       u_param,
                float       v_param,
                uint        seed,
                Vec4<float> bbox)
{
  Array array(shape);

  auto run = clwrapper::Run("voronoise");

  run.bind_buffer<float>("array", array.vector);
  run.bind_arguments(array.shape.x,
                     array.shape.y,
                     kw.x,
                     kw.y,
                     u_param,
                     v_param,
                     seed,
                     bbox);

  run.write_buffer("array");
  run.execute({array.shape.x, array.shape.y});
  run.read_buffer("array");

  return array;
}

} // namespace hmap::gpu
#endif
