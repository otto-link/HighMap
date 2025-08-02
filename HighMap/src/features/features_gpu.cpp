/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/features.hpp"
#include "highmap/filters.hpp"
#include "highmap/math.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

Array local_median_deviation(const Array &array, int ir)
{
  Array mean = gpu::mean_local(array, ir);
  Array med = gpu::median_pseudo(array, ir); // TODO exact
  return abs(mean - med);
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

Array relative_elevation(const Array &array, int ir)
{
  Array amin = gpu::minimum_local(array, ir);
  Array amax = gpu::maximum_local(array, ir);

  gpu::smooth_cpulse(amin, ir);
  gpu::smooth_cpulse(amax, ir);

  return (array - amin) / (amax - amin + std::numeric_limits<float>::min());
}

Array ruggedness(const Array &array, int ir)
{
  Array rg(array.shape);

  auto run = clwrapper::Run("ruggedness");

  run.bind_imagef("array",
                  const_cast<std::vector<float> &>(array.vector),
                  array.shape.x,
                  array.shape.y);
  run.bind_imagef("out", rg.vector, array.shape.x, array.shape.y, true);
  run.bind_arguments(array.shape.x, array.shape.y, ir);

  run.execute({array.shape.x, array.shape.y});

  run.read_imagef("out");

  return rg;
}

Array rugosity(const Array &z, int ir, bool convex)
{
  Array z_avg(z.shape);
  Array z_std(z.shape);
  Array z_skw(z.shape);
  Array zf = z;
  float tol = 1e-30f;

  // use a kernels only for filtering
  gpu::smooth_cpulse(zf, 2 * ir);
  zf = z - zf;
  z_avg = zf;
  gpu::smooth_cpulse(z_avg, ir);
  z_std = (zf - z_avg) * (zf - z_avg);
  gpu::smooth_cpulse(z_std, ir);
  z_skw = (zf - z_avg) * (zf - z_avg) * (zf - z_avg);

  // last part with dedicated kernel
  auto run = clwrapper::Run("rugosity_post");

  run.bind_buffer("z_skw", z_skw.vector);
  run.bind_buffer("z_std", z_std.vector);
  run.bind_arguments(z.shape.x, z.shape.y, tol, convex ? 1 : 0);

  run.write_buffer("z_skw");
  run.write_buffer("z_std");

  run.execute({z.shape.x, z.shape.y});

  run.read_buffer("z_skw");

  return z_skw;
}

Array std_local(const Array &array, int ir)
{
  // NB - use Gaussian windowing instead of a real arithmetic averaging
  Array mean = array;
  gpu::smooth_cpulse(mean, ir);

  // use mean to store (array - mean)^2
  mean -= array;
  mean *= mean;
  gpu::smooth_cpulse(mean, ir);
  Array std = sqrt(mean);

  return std;
}

Array z_score(const Array &array, int ir)
{
  // NB - use Gaussian windowing instead of a real arithmetic averaging
  Array mean = array;
  gpu::smooth_cpulse(mean, ir);

  // use mean to store (array - mean)^2
  mean -= array;
  mean *= mean;
  gpu::smooth_cpulse(mean, ir);
  Array std = sqrt(mean);

  return (array - mean) / std;
}

} // namespace hmap::gpu
