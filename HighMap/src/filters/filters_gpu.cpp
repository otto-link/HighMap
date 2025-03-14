/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/curvature.hpp"
#include "highmap/filters.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

void expand(Array &array, int ir)
{
  Array kernel = cubic_pulse({2 * ir + 1, 2 * ir + 1});
  gpu::expand(array, kernel);
}

void expand(Array &array, int ir, const Array *p_mask)
{
  Array kernel = cubic_pulse({2 * ir + 1, 2 * ir + 1});

  if (!p_mask)
  {
    gpu::expand(array, kernel);
  }
  else
  {
    gpu::expand(array, kernel, p_mask);
  }
}

void expand(Array &array, const Array &kernel)
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

void expand(Array &array, const Array &kernel, const Array *p_mask)
{
  if (!p_mask)
  {
    gpu::expand(array, kernel);
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

void gamma_correction_local(Array &array, float gamma, int ir, float k)
{
  Array amin = gpu::minimum_local(array, ir);
  Array amax = gpu::maximum_local(array, ir);

  gpu::smooth_cpulse(amin, ir);
  gpu::smooth_cpulse(amax, ir);

  if (k != 0) // with smoothing
  {
    for (int j = 0; j < array.shape.y; j++)
      for (int i = 0; i < array.shape.x; i++)
      {
        float v = std::abs(array(i, j) - amin(i, j)) /
                  (amax(i, j) - amin(i, j) + 1e-30);
        v = std::sqrt(v * v + k);
        array(i, j) = std::pow(v, gamma) * (amax(i, j) - amin(i, j)) +
                      amin(i, j);
      }
  }
  else // without smoothing
  {
    for (int j = 0; j < array.shape.y; j++)
      for (int i = 0; i < array.shape.x; i++)
      {
        float v = std::abs(array(i, j) - amin(i, j)) /
                  (amax(i, j) - amin(i, j) + 1e-30);
        array(i, j) = std::pow(v, gamma) * (amax(i, j) - amin(i, j)) +
                      amin(i, j);
      }
  }
}

void gamma_correction_local(Array       &array,
                            float        gamma,
                            int          ir,
                            const Array *p_mask,
                            float        k)
{
  if (!p_mask)
    gpu::gamma_correction_local(array, gamma, ir, k);
  else
  {
    Array array_f = array;
    gpu::gamma_correction_local(array_f, gamma, ir, k);
    array = lerp(array, array_f, *(p_mask));
  }
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

void laplace(Array &array, const Array *p_mask, float sigma, int iterations)
{
  if (!p_mask)
  {
    gpu::laplace(array, sigma, iterations);
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

Array maximum_local(const Array &array, int ir)
{
  auto run = clwrapper::Run("maximum_local");

  Array array_out = array;

  run.bind_imagef("in", array_out.vector, array.shape.x, array.shape.y);
  run.bind_imagef("out", array_out.vector, array.shape.x, array.shape.y, true);
  run.bind_arguments(array.shape.x, array.shape.y, ir, 0);

  // row pass
  run.execute({array.shape.x, array.shape.y});
  run.read_imagef("out");

  // col pass
  run.write_imagef("in");
  run.set_argument(5, 1); // pass_number
  run.execute({array.shape.x, array.shape.y});
  run.read_imagef("out");

  return array_out;
}

Array maximum_local_disk(const Array &array, int ir)
{
  Array kernel = disk({2 * ir + 1, 2 * ir + 1});
  Array array_out = array;
  gpu::expand(array_out, kernel);
  return array_out;
}

Array mean_local(const Array &array, int ir)
{
  Array array_out = array;

  auto run = clwrapper::Run("mean_local");

  run.bind_imagef("in", array_out.vector, array.shape.x, array.shape.y);
  run.bind_imagef("out", array_out.vector, array.shape.x, array.shape.y, true);
  run.bind_arguments(array.shape.x, array.shape.y, ir, 0);

  run.set_argument(5, 0); // row pass
  run.execute({array.shape.x, array.shape.y});
  run.read_imagef("out");

  run.set_argument(5, 1); // col pass
  run.write_imagef("in");
  run.execute({array.shape.x, array.shape.y});
  run.read_imagef("out");

  return array_out;
}

Array mean_shift(const Array &array,
                 int          ir,
                 float        talus,
                 int          iterations,
                 bool         talus_weighted)
{
  const Vec2<int> shape = array.shape;
  Array           array_next = Array(shape);
  Array           array_prev = array;

  auto run = clwrapper::Run("mean_shift");

  run.bind_imagef("in", array_prev.vector, shape.x, shape.y);
  run.bind_imagef("out", array_next.vector, shape.x, shape.y, true);
  run.bind_arguments(shape.x, shape.y, ir, talus, talus_weighted ? 1 : 0);

  for (int it = 0; it < iterations; it++)
  {
    run.execute({shape.x, shape.y});
    run.read_imagef("out");

    if (iterations > 1)
    {
      array_prev = array_next;
      run.write_imagef("in");
    }
  }

  run.read_imagef("out");

  return array_next;
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

void median_3x3(Array &array, const Array *p_mask)
{
  if (!p_mask)
    gpu::median_3x3(array);
  else
  {
    Array array_f = array;
    gpu::median_3x3(array_f);
    array = lerp(array, array_f, *(p_mask));
  }
}

Array minimum_local(const Array &array, int ir)
{
  return -gpu::maximum_local(-array, ir);
}

Array minimum_local_disk(const Array &array, int ir)
{
  Array kernel = disk({2 * ir + 1, 2 * ir + 1});
  Array array_out = array;
  gpu::shrink(array_out, kernel);
  return array_out;
}

void normal_displacement(Array &array, float amount, int ir, bool reverse)
{
  auto run = clwrapper::Run("normal_displacement");

  Array array_f = array;
  if (ir > 0) gpu::smooth_cpulse(array_f, ir);

  if (reverse) amount *= -1.f;

  run.bind_imagef("array", array.vector, array.shape.x, array.shape.y);
  run.bind_imagef("array_f", array_f.vector, array.shape.x, array.shape.y);
  run.bind_imagef("out", array.vector, array.shape.x, array.shape.y, true);
  run.bind_arguments(array.shape.x, array.shape.y, amount);

  run.execute({array.shape.x, array.shape.y});

  run.read_imagef("out");
}

void normal_displacement(Array       &array,
                         const Array *p_mask,
                         float        amount,
                         int          ir,
                         bool         reverse)
{
  if (!p_mask)
  {
    gpu::normal_displacement(array, amount, ir, reverse);
  }
  else
  {
    auto run = clwrapper::Run("normal_displacement_masked");

    Array array_f = array;
    if (ir > 0) gpu::smooth_cpulse(array_f, ir);

    if (reverse) amount *= -1.f;

    run.bind_imagef("array", array.vector, array.shape.x, array.shape.y);
    run.bind_imagef("array_f", array_f.vector, array.shape.x, array.shape.y);
    run.bind_imagef("mask", p_mask->vector, array.shape.x, array.shape.y);
    run.bind_imagef("out", array.vector, array.shape.x, array.shape.y, true);
    run.bind_arguments(array.shape.x, array.shape.y, amount);

    run.execute({array.shape.x, array.shape.y});

    run.read_imagef("out");
  }
}

void plateau(Array &array, const Array *p_mask, int ir, float factor)
{
  Array amin = gpu::minimum_local(array, ir);
  Array amax = gpu::maximum_local(array, ir);

  gpu::smooth_cpulse(amin, ir);
  gpu::smooth_cpulse(amax, ir);

  // last part
  auto run = clwrapper::Run("plateau_post");

  run.bind_buffer<float>("array", array.vector);
  run.bind_buffer<float>("amin", amin.vector);
  run.bind_buffer<float>("amax", amax.vector);

  helper_bind_optional_buffer(run, "mask", p_mask);

  run.bind_arguments(array.shape.x, array.shape.y, factor, p_mask ? 1 : 0);

  run.write_buffer("array");
  run.write_buffer("amin");
  run.write_buffer("amax");

  run.execute({array.shape.x, array.shape.y});

  run.read_buffer("array");
}

void plateau(Array &array, int ir, float factor)
{
  gpu::plateau(array, nullptr, ir, factor);
}

void shrink(Array &array, int ir)
{
  Array kernel = cubic_pulse({2 * ir + 1, 2 * ir + 1});
  gpu::shrink(array, kernel);
}

void shrink(Array &array, int ir, const Array *p_mask)
{
  Array kernel = cubic_pulse({2 * ir + 1, 2 * ir + 1});

  if (!p_mask)
  {
    gpu::shrink(array, kernel);
  }
  else
  {
    gpu::shrink(array, kernel, p_mask);
  }
}

void shrink(Array &array, const Array &kernel)
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

void shrink(Array &array, const Array &kernel, const Array *p_mask)
{
  if (!p_mask)
  {
    gpu::shrink(array, kernel);
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

void smooth_cpulse(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
  {
    gpu::smooth_cpulse(array, ir);
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

  gpu::smooth_cpulse(array, ir);
  array = gpu::maximum_smooth(array, array_bckp, k);

  if (p_deposition_map) *p_deposition_map = maximum(array - array_bckp, 0.f);
}

void smooth_fill(Array       &array,
                 int          ir,
                 const Array *p_mask,
                 float        k,
                 Array       *p_deposition_map)
{
  Array array_bckp = array;

  gpu::smooth_cpulse(array, ir, p_mask);
  array = gpu::maximum_smooth(array, array_bckp, k);

  if (p_deposition_map) *p_deposition_map = maximum(array - array_bckp, 0.f);
}

void smooth_fill_holes(Array &array, int ir)
{
  Array array_smooth = array;
  gpu::smooth_cpulse(array_smooth, ir);

  // mask based on concave regions
  Array mask = -curvature_mean(array_smooth);
  clamp_min(mask, 0.f);
  make_binary(mask);

  int ic = (int)((float)ir / 2.f);
  if (ic > 1) gpu::smooth_cpulse(mask, ic);

  array = lerp(array, array_smooth, mask);
}

void smooth_fill_holes(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
    gpu::smooth_fill_holes(array, ir);
  else
  {
    Array array_f = array;
    gpu::smooth_fill_holes(array_f, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

void smooth_fill_smear_peaks(Array &array, int ir)
{
  Array array_smooth = array;
  gpu::smooth_cpulse(array_smooth, ir);

  // mask based on concave regions
  Array mask = curvature_mean(array_smooth);
  clamp_min(mask, 0.f);
  make_binary(mask);

  int ic = (int)((float)ir / 2.f);
  if (ic > 0) gpu::smooth_cpulse(mask, ic);

  array = lerp(array, array_smooth, mask);
}

void smooth_fill_smear_peaks(Array &array, int ir, const Array *p_mask)
{
  if (!p_mask)
    gpu::smooth_fill_smear_peaks(array, ir);
  else
  {
    Array array_f = array;
    gpu::smooth_fill_smear_peaks(array_f, ir);
    array = lerp(array, array_f, *(p_mask));
  }
}

} // namespace hmap::gpu
