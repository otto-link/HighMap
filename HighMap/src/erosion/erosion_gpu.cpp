/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/math.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

void hydraulic_particle(Array &z,
                        int    nparticles,
                        int    seed,
                        Array *p_bedrock,
                        Array *p_moisture_map,
                        Array *p_erosion_map,
                        Array *p_deposition_map,
                        float  c_capacity,
                        float  c_erosion,
                        float  c_deposition,
                        float  c_inertia,
                        float  drag_rate,
                        float  evap_rate,
                        bool   post_filtering)
{
  Array z_bckp = Array();
  if ((p_erosion_map != nullptr) | (p_deposition_map != nullptr)) z_bckp = z;

  // kernel
  auto run = clwrapper::Run("hydraulic_particle");

  run.bind_buffer<float>("z", z.vector);

  helper_bind_optional_buffer(run, "bedrock", p_bedrock);
  helper_bind_optional_buffer(run, "moisture_map", p_moisture_map);

  run.bind_arguments(z.shape.x,
                     z.shape.y,
                     nparticles,
                     seed,
                     c_capacity,
                     c_erosion,
                     c_deposition,
                     c_inertia,
                     drag_rate,
                     evap_rate,
                     p_bedrock ? 1 : 0,
                     p_moisture_map ? 1 : 0);

  run.write_buffer("z");

  run.execute(nparticles);

  run.read_buffer("z");

  // post-filter
  if (post_filtering)
  {
    float sigma = 0.25f;
    int   iterations = 1;
    gpu::laplace(z, sigma, iterations);
  }

  // splatmaps
  if (p_erosion_map)
  {
    *p_erosion_map = z_bckp - z;
    clamp_min(*p_erosion_map, 0.f);
  }

  if (p_deposition_map)
  {
    *p_deposition_map = z - z_bckp;
    clamp_min(*p_deposition_map, 0.f);
  }
}

void hydraulic_particle(Array &z,
                        Array *p_mask,
                        int    nparticles,
                        int    seed,
                        Array *p_bedrock,
                        Array *p_moisture_map,
                        Array *p_erosion_map,
                        Array *p_deposition_map,
                        float  c_capacity,
                        float  c_erosion,
                        float  c_deposition,
                        float  c_inertia,
                        float  drag_rate,
                        float  evap_rate,
                        bool   post_filtering)
{
  if (!p_mask)
    gpu::hydraulic_particle(z,
                            nparticles,
                            seed,
                            p_bedrock,
                            p_moisture_map,
                            p_erosion_map,
                            p_deposition_map,
                            c_capacity,
                            c_erosion,
                            c_deposition,
                            c_inertia,
                            drag_rate,
                            evap_rate,
                            post_filtering);
  else
  {
    Array z_f = z;
    gpu::hydraulic_particle(z_f,
                            nparticles,
                            seed,
                            p_bedrock,
                            p_moisture_map,
                            p_erosion_map,
                            p_deposition_map,
                            c_capacity,
                            c_erosion,
                            c_deposition,
                            c_inertia,
                            drag_rate,
                            evap_rate,
                            post_filtering);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap::gpu
