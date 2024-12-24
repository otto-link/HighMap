/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#ifdef ENABLE_OPENCL

#include "highmap/opencl/gpu_opencl.hpp"

namespace hmap::gpu
{

void helper_bind_optional_buffer(clwrapper::Run    &run,
                                 const std::string &id,
                                 Array             *p_array)
{
  std::vector<float> dummy_vector(1);

  if (p_array)
  {
    run.bind_buffer<float>(id, p_array->vector);
    run.write_buffer(id);
  }
  else
    run.bind_buffer<float>(id, dummy_vector);
}

bool init_opencl()
{
  if (!clwrapper::DeviceManager::get_instance().is_ready()) return false;

  // load and build kernels
  const std::string code =
#include "kernels/_common_index.cl"
#include "kernels/_common_math.cl"
#include "kernels/_common_rand.cl"
#include "kernels/_common_sort.cl"
  //
#include "kernels/expand.cl"
#include "kernels/gradient_norm.cl"
#include "kernels/hydraulic_particle.cl"
#include "kernels/laplace.cl"
#include "kernels/maximum_local.cl"
#include "kernels/maximum_smooth.cl"
#include "kernels/mean_local.cl"
#include "kernels/median_3x3.cl"
#include "kernels/minimum_smooth.cl"
#include "kernels/noise.cl"
#include "kernels/normal_displacement.cl"
#include "kernels/plateau.cl"
#include "kernels/rugosity.cl"
#include "kernels/skeleton.cl"
#include "kernels/smooth_cpulse.cl"
#include "kernels/thermal.cl"
#include "kernels/warp.cl"
  //
  // kernels only available on GPU
#include "kernels/gabor_wave.cl"
#include "kernels/gavoronoise.cl"
#include "kernels/voronoi_edge_distance.cl"
#include "kernels/voronoise.cl"
      ;

  clwrapper::KernelManager::get_instance().add_kernel(code);

  return true;
}

} // namespace hmap::gpu
#endif
