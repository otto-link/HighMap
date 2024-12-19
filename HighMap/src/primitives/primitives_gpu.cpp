/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#ifdef ENABLE_OPENCL

#include "highmap/array.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

void helper_bind_optional_buffers(clwrapper::Run &run,
                                  Array          *p_ctrl_param,
                                  Array          *p_noise_x,
                                  Array          *p_noise_y)
{
  std::vector<float> dummy_vector(1);

  if (p_ctrl_param)
  {
    run.bind_buffer<float>("ctrl_param", p_ctrl_param->vector);
    run.write_buffer("ctrl_param");
  }
  else
    run.bind_buffer<float>("ctrl_param", dummy_vector);

  if (p_noise_x)
  {
    run.bind_buffer<float>("noise_x", p_noise_x->vector);
    run.write_buffer("noise_x");
  }
  else
    run.bind_buffer<float>("noise_x", dummy_vector);

  if (p_noise_y)
  {
    run.bind_buffer<float>("noise_y", p_noise_y->vector);
    run.write_buffer("noise_y");
  }
  else
    run.bind_buffer<float>("noise_y", dummy_vector);
}

void helper_bind_optional_buffers(clwrapper::Run &run,
                                  Array          *p_noise_x,
                                  Array          *p_noise_y)
{
  std::vector<float> dummy_vector(1);

  if (p_noise_x)
  {
    run.bind_buffer<float>("noise_x", p_noise_x->vector);
    run.write_buffer("noise_x");
  }
  else
    run.bind_buffer<float>("noise_x", dummy_vector);

  if (p_noise_y)
  {
    run.bind_buffer<float>("noise_y", p_noise_y->vector);
    run.write_buffer("noise_y");
  }
  else
    run.bind_buffer<float>("noise_y", dummy_vector);
}

Array gabor_wave(Vec2<int> shape, Vec2<float> kw, uint seed, Vec4<float> bbox)
{
  Array array(shape);

  auto run = clwrapper::Run("gabor_wave");

  run.bind_buffer<float>("array", array.vector);
  run.bind_arguments(array.shape.x, array.shape.y, kw.x, kw.y, seed, bbox);

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
                     Array      *p_ctrl_param,
                     Array      *p_noise_x,
                     Array      *p_noise_y,
                     Vec4<float> bbox)
{
  Array array(shape);

  auto run = clwrapper::Run("gabor_wave_fbm");

  run.bind_buffer<float>("array", array.vector);
  helper_bind_optional_buffers(run, p_ctrl_param, p_noise_x, p_noise_y);

  run.bind_arguments(array.shape.x,
                     array.shape.y,
                     kw.x,
                     kw.y,
                     seed,
                     octaves,
                     weight,
                     persistence,
                     lacunarity,
                     p_ctrl_param ? 1 : 0,
                     p_noise_x ? 1 : 0,
                     p_noise_y ? 1 : 0,
                     bbox);

  run.execute({array.shape.x, array.shape.y});
  run.read_buffer("array");

  return array;
}

Array voronoise(Vec2<int>   shape,
                Vec2<float> kw,
                float       u_param,
                float       v_param,
                uint        seed,
                Array      *p_noise_x,
                Array      *p_noise_y,
                Vec4<float> bbox)
{
  Array array(shape);

  auto run = clwrapper::Run("voronoise");

  run.bind_buffer<float>("array", array.vector);
  helper_bind_optional_buffers(run, p_noise_x, p_noise_y);

  run.bind_arguments(array.shape.x,
                     array.shape.y,
                     kw.x,
                     kw.y,
                     u_param,
                     v_param,
                     seed,
                     p_noise_x ? 1 : 0,
                     p_noise_y ? 1 : 0,
                     bbox);

  run.execute({array.shape.x, array.shape.y});
  run.read_buffer("array");

  return array;
}

} // namespace hmap::gpu
#endif
