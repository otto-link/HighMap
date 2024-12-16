/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#ifdef ENABLE_OPENCL

#include "highmap/kernels.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

void expand(Array &array, int ir)
{
  Array kernel = cubic_pulse({2 * ir + 1, 2 * ir + 1});
  expand(array, kernel);
}

void expand(Array &array, int ir, Array *p_mask)
{
  Array kernel = cubic_pulse({2 * ir + 1, 2 * ir + 1});

  if (!p_mask)
  {
    expand(array, kernel);
  }
  else
  {
    expand(array, kernel, p_mask);
  }
}

void expand(Array &array, Array &kernel)
{
  auto run = clwrapper::Run("expand");

  run.bind_imagef("z", array.vector, array.shape.x, array.shape.y);
  run.bind_imagef("weights", kernel.vector, kernel.shape.x, kernel.shape.y);
  run.bind_imagef("out",
                  array.vector,
                  array.shape.x,
                  array.shape.y,
                  true); // out
  run.bind_arguments(array.shape.x,
                     array.shape.y,
                     kernel.shape.x,
                     kernel.shape.y);

  run.execute({array.shape.x, array.shape.y});

  run.read_imagef("out");
}

void expand(Array &array, Array &kernel, Array *p_mask)
{
  if (!p_mask)
  {
    expand(array, kernel);
  }
  else
  {
    auto run = clwrapper::Run("expand_masked");

    run.bind_imagef("z", array.vector, array.shape.x, array.shape.y);
    run.bind_imagef("weights", kernel.vector, kernel.shape.x, kernel.shape.y);
    run.bind_imagef("mask", p_mask->vector, p_mask->shape.x, p_mask->shape.y);
    run.bind_imagef("out",
                    array.vector,
                    array.shape.x,
                    array.shape.y,
                    true); // out
    run.bind_arguments(array.shape.x,
                       array.shape.y,
                       kernel.shape.x,
                       kernel.shape.y);

    run.execute({array.shape.x, array.shape.y});

    run.read_imagef("out");
  }
}

Array gradient_norm(const Array &array)
{
  Array dm(array.shape);

  auto run = clwrapper::Run("gradient_norm");

  run.bind_buffer<float>("array",
                         const_cast<std::vector<float> &>(array.vector));
  run.bind_buffer<float>("dm", dm.vector);
  run.bind_arguments(array.shape.x, array.shape.y);

  run.write_buffer("array");

  run.execute({array.shape.x, array.shape.y});

  run.read_buffer("dm");

  return dm;
}

void laplace(Array &array, float sigma, int iterations)
{
  auto run = clwrapper::Run("laplace");

  run.bind_buffer<float>("array", array.vector);
  run.bind_arguments(array.shape.x, array.shape.y, sigma);

  run.write_buffer("array");

  for (int it = 0; it < iterations; it++)
    run.execute({array.shape.x, array.shape.y});

  run.read_buffer("array");
}

void laplace(Array &array, Array *p_mask, float sigma, int iterations)
{
  if (!p_mask)
  {
    laplace(array, sigma, iterations);
  }
  else
  {
    auto run = clwrapper::Run("laplace_masked");

    run.bind_buffer<float>("array", array.vector);
    run.bind_buffer<float>("mask", p_mask->vector);
    run.bind_arguments(array.shape.x, array.shape.y, sigma);

    run.write_buffer("array");
    run.write_buffer("mask");

    for (int it = 0; it < iterations; it++)
      run.execute({array.shape.x, array.shape.y});

    run.read_buffer("array");
  }
}

Array maximum_local_disk(const Array &array, int ir)
{
  Array kernel = disk({2 * ir + 1, 2 * ir + 1});
  Array array_out = array;
  expand(array_out, kernel);
  return array_out;
}

Array maximum_smooth(const Array &array1, const Array &array2, float k)
{
  Array array_out = array1;

  auto run = clwrapper::Run("maximum_smooth");

  run.bind_buffer("array1", array_out.vector);
  run.bind_buffer("array2", const_cast<std::vector<float> &>(array2.vector));
  run.bind_arguments(array1.shape.x, array1.shape.y, k);

  run.write_buffer("array1");
  run.write_buffer("array2");

  run.execute({array1.shape.x, array1.shape.y});

  run.read_buffer("array1");

  return array_out;
}

Array minimum_local_disk(const Array &array, int ir)
{
  Array kernel = disk({2 * ir + 1, 2 * ir + 1});
  Array array_out = array;
  shrink(array_out, kernel);
  return array_out;
}

Array minimum_smooth(const Array &array1, const Array &array2, float k)
{
  Array array_out = array1;

  auto run = clwrapper::Run("minimum_smooth");

  run.bind_buffer("array1", array_out.vector);
  run.bind_buffer("array2", const_cast<std::vector<float> &>(array2.vector));
  run.bind_arguments(array1.shape.x, array1.shape.y, k);

  run.write_buffer("array1");
  run.write_buffer("array2");

  run.execute({array1.shape.x, array1.shape.y});

  run.read_buffer("array1");

  return array_out;
}

void median_3x3(Array &array)
{
  auto run = clwrapper::Run("median_3x3");

  run.bind_imagef("in", array.vector, array.shape.x, array.shape.y);
  run.bind_imagef("out", array.vector, array.shape.x, array.shape.y, true);
  run.bind_arguments(array.shape.x, array.shape.y);

  run.execute({array.shape.x, array.shape.y});

  run.read_imagef("out");
}

void shrink(Array &array, int ir)
{
  Array kernel = cubic_pulse({2 * ir + 1, 2 * ir + 1});
  shrink(array, kernel);
}

void shrink(Array &array, int ir, Array *p_mask)
{
  Array kernel = cubic_pulse({2 * ir + 1, 2 * ir + 1});

  if (!p_mask)
  {
    shrink(array, kernel);
  }
  else
  {
    shrink(array, kernel, p_mask);
  }
}

void shrink(Array &array, Array &kernel)
{
  float amax = array.max();
  array *= -1.f; // array <- amax - array;
  array += amax;

  auto run = clwrapper::Run("expand");

  run.bind_imagef("z", array.vector, array.shape.x, array.shape.y);
  run.bind_imagef("weights", kernel.vector, kernel.shape.x, kernel.shape.y);
  run.bind_imagef("out",
                  array.vector,
                  array.shape.x,
                  array.shape.y,
                  true); // out
  run.bind_arguments(array.shape.x,
                     array.shape.y,
                     kernel.shape.x,
                     kernel.shape.y);

  run.execute({array.shape.x, array.shape.y});

  run.read_imagef("out");

  array *= -1.f; // array <- amax - array;
  array += amax;
}

void shrink(Array &array, Array &kernel, Array *p_mask)
{
  if (!p_mask)
  {
    shrink(array, kernel);
  }
  else
  {
    float amax = array.max();
    array *= -1.f; // array <- amax - array;
    array += amax;

    auto run = clwrapper::Run("expand_masked");

    run.bind_imagef("z", array.vector, array.shape.x, array.shape.y);
    run.bind_imagef("weights", kernel.vector, kernel.shape.x, kernel.shape.y);
    run.bind_imagef("mask", p_mask->vector, p_mask->shape.x, p_mask->shape.y);
    run.bind_imagef("out",
                    array.vector,
                    array.shape.x,
                    array.shape.y,
                    true); // out
    run.bind_arguments(array.shape.x,
                       array.shape.y,
                       kernel.shape.x,
                       kernel.shape.y);

    run.execute({array.shape.x, array.shape.y});

    run.read_imagef("out");

    array *= -1.f; // array <- amax - array;
    array += amax;
  }
}

void smooth_cpulse(Array &array, int ir)
{
  const int          nk = 2 * ir + 1;
  std::vector<float> kernel_1d = cubic_pulse_1d(nk);

  auto run = clwrapper::Run("smooth_cpulse");

  run.bind_imagef("in", array.vector, array.shape.x, array.shape.y);
  run.bind_imagef("weights", kernel_1d, nk, 1);
  run.bind_imagef("out",
                  array.vector,
                  array.shape.x,
                  array.shape.y,
                  true); // out
  run.bind_arguments(array.shape.x, array.shape.y, ir);

  int pass_nb;

  pass_nb = 0; // x
  run.set_argument(6, pass_nb);
  run.execute({array.shape.x, array.shape.y});

  run.read_imagef("out");
  run.write_imagef("in");

  pass_nb = 1; // y
  run.set_argument(6, pass_nb);
  run.execute({array.shape.x, array.shape.y});

  run.read_imagef("out");
}

void smooth_cpulse(Array &array, int ir, Array *p_mask)
{
  if (!p_mask)
  {
    smooth_cpulse(array, ir);
  }
  else
  {
    const int          nk = 2 * ir + 1;
    std::vector<float> kernel_1d = cubic_pulse_1d(nk);

    auto run = clwrapper::Run("smooth_cpulse_masked");

    run.bind_imagef("in", array.vector, array.shape.x, array.shape.y);
    run.bind_imagef("weights", kernel_1d, nk, 1);
    run.bind_imagef("mask", p_mask->vector, p_mask->shape.x, p_mask->shape.y);
    run.bind_imagef("out",
                    array.vector,
                    array.shape.x,
                    array.shape.y,
                    true); // out
    run.bind_arguments(array.shape.x, array.shape.y, ir);

    int pass_nb;

    pass_nb = 0; // x
    run.set_argument(7, pass_nb);
    run.execute({array.shape.x, array.shape.y});

    run.read_imagef("out");
    run.write_imagef("in");

    pass_nb = 1; // y
    run.set_argument(7, pass_nb);
    run.execute({array.shape.x, array.shape.y});

    run.read_imagef("out");
  }
}

void smooth_fill(Array &array, int ir, float k, Array *p_deposition_map)
{
  Array array_bckp = array;

  smooth_cpulse(array, ir);
  array = maximum_smooth(array, array_bckp, k);

  if (p_deposition_map)
  {
    *p_deposition_map = array - array_bckp;
    // clamp_min(*p_deposition_map, 0.f); // TODO
  }
}

void smooth_fill(Array &array,
                 int    ir,
                 Array *p_mask,
                 float  k,
                 Array *p_deposition_map)
{
  Array array_bckp = array;

  smooth_cpulse(array, ir, p_mask);
  array = maximum_smooth(array, array_bckp, k);

  if (p_deposition_map)
  {
    *p_deposition_map = array - array_bckp;
    // clamp_min(*p_deposition_map, 0.f); // TODO
  }
}

void thermal(Array &z, const Array &talus, int iterations)
{
  auto run = clwrapper::Run("thermal");

  Array z2(z.shape);

  run.bind_imagef("z", z.vector, z.shape.x, z.shape.y);
  run.bind_imagef("talus",
                  const_cast<std::vector<float> &>(talus.vector),
                  z.shape.x,
                  z.shape.y);
  run.bind_imagef("out", z.vector, z.shape.x, z.shape.y, true); // out
  run.bind_arguments(z.shape.x, z.shape.y);

  for (int it = 0; it < 50; it++)
  {
    run.write_imagef("z");
    run.execute({z.shape.x, z.shape.y});
    run.read_imagef("out");
  }
}

void thermal_bf(Array &z, const Array &talus, int iterations)
{
  auto run = clwrapper::Run("thermal_bf");

  run.bind_buffer<float>("z", z.vector);
  run.bind_buffer<float>("talus",
                         const_cast<std::vector<float> &>(talus.vector));
  run.bind_arguments(z.shape.x, z.shape.y, 0);

  run.write_buffer("z");
  run.write_buffer("talus");

  run.execute({z.shape.x, z.shape.y});

  run.read_buffer("z");

  for (int it = 0; it < 100; it++)
  {
    run.set_argument(4, it);
    run.execute({z.shape.x, z.shape.y});
    // run.read_buffer("z");
    // fill_borders(z);
    // run.write_buffer("z");
  }

  run.read_buffer("z");
}

void warp(Array &array, Array *p_dx, Array *p_dy)
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
#endif
