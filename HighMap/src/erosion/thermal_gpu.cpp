/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/boundary.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

void thermal(Array       &z,
             const Array &talus,
             int          iterations,
             Array       *p_bedrock,
             Array       *p_deposition_map)
{
  Array z_bckp = Array();
  if (p_deposition_map != nullptr) z_bckp = z;

  if (p_bedrock)
  {
    auto run = clwrapper::Run("thermal_with_bedrock");

    run.bind_buffer<float>("z", z.vector);
    run.bind_buffer<float>("talus",
                           const_cast<std::vector<float> &>(talus.vector));
    run.bind_buffer<float>("bedrock", p_bedrock->vector);
    run.bind_arguments(z.shape.x, z.shape.y, 0);

    run.write_buffer("z");
    run.write_buffer("talus");
    run.write_buffer("bedrock");

    for (int it = 0; it < iterations; it++)
    {
      run.set_argument(5, it);
      run.execute({z.shape.x, z.shape.y});
    }

    run.read_buffer("z");
  }
  else
  {
    auto run = clwrapper::Run("thermal");

    run.bind_buffer<float>("z", z.vector);
    run.bind_buffer<float>("talus",
                           const_cast<std::vector<float> &>(talus.vector));
    run.bind_arguments(z.shape.x, z.shape.y, 0);

    run.write_buffer("z");
    run.write_buffer("talus");

    for (int it = 0; it < iterations; it++)
    {
      run.set_argument(4, it);
      run.execute({z.shape.x, z.shape.y});
    }

    run.read_buffer("z");
  }

  extrapolate_borders(z);

  if (p_deposition_map) *p_deposition_map = maximum(z - z_bckp, 0.f);
}

void thermal(Array       &z,
             Array       *p_mask,
             const Array &talus,
             int          iterations,
             Array       *p_bedrock,
             Array       *p_deposition_map)
{
  if (!p_mask)
    gpu::thermal(z, talus, iterations, p_bedrock, p_deposition_map);
  else
  {
    Array z_f = z;
    gpu::thermal(z_f, talus, iterations, p_bedrock, p_deposition_map);
    z = lerp(z, z_f, *(p_mask));
  }
}

void thermal(Array &z,
             float  talus,
             int    iterations,
             Array *p_bedrock,
             Array *p_deposition_map)
{
  Array talus_map(z.shape, talus);
  gpu::thermal(z, talus_map, iterations, p_bedrock, p_deposition_map);
}

void thermal_auto_bedrock(Array       &z,
                          const Array &talus,
                          int          iterations,
                          Array       *p_deposition_map)
{
  Array z_bckp = z;
  Array bedrock(z.shape);

  auto run = clwrapper::Run("thermal_auto_bedrock");

  run.bind_buffer<float>("z", z.vector);
  run.bind_buffer<float>("talus",
                         const_cast<std::vector<float> &>(talus.vector));
  run.bind_buffer<float>("bedrock", bedrock.vector);
  run.bind_buffer<float>("z0", z_bckp.vector);
  run.bind_arguments(z.shape.x, z.shape.y, 0);

  run.write_buffer("z");
  run.write_buffer("talus");
  run.write_buffer("bedrock");
  run.write_buffer("z0");

  for (int it = 0; it < iterations; it++)
  {
    run.set_argument(6, it);
    run.execute({z.shape.x, z.shape.y});
  }

  run.read_buffer("z");
  extrapolate_borders(z);

  if (p_deposition_map) *p_deposition_map = maximum(z - z_bckp, 0.f);
}

void thermal_auto_bedrock(Array &z,
                          float  talus,
                          int    iterations,
                          Array *p_deposition_map)
{
  Array talus_map(z.shape, talus);
  gpu::thermal_auto_bedrock(z, talus_map, iterations, p_deposition_map);
}

void thermal_auto_bedrock(Array       &z,
                          Array       *p_mask,
                          const Array &talus,
                          int          iterations,
                          Array       *p_deposition_map)
{
  if (!p_mask)
    gpu::thermal_auto_bedrock(z, talus, iterations, p_deposition_map);
  else
  {
    Array z_f = z;
    gpu::thermal_auto_bedrock(z_f, talus, iterations, p_deposition_map);
    z = lerp(z, z_f, *(p_mask));
  }
}

void thermal_inflate(Array &z, const Array &talus, int iterations)
{
  auto run = clwrapper::Run("thermal_inflate");

  run.bind_buffer<float>("z", z.vector);
  run.bind_buffer<float>("talus", talus.vector);
  run.bind_arguments(z.shape.x, z.shape.y);

  run.write_buffer("z");
  run.write_buffer("talus");

  for (int it = 0; it < iterations; it++)
    run.execute({z.shape.x, z.shape.y});

  run.read_buffer("z");
  extrapolate_borders(z);
}

void thermal_inflate(Array       &z,
                     const Array *p_mask,
                     const Array &talus,
                     int          iterations)
{
  if (!p_mask)
    gpu::thermal_inflate(z, talus, iterations);
  else
  {
    Array z_f = z;
    gpu::thermal_inflate(z_f, talus, iterations);
    z = lerp(z, z_f, *(p_mask));
  }
}

void thermal_rib(Array &z, int iterations, Array *p_bedrock)
{
  auto run = clwrapper::Run("thermal_rib");

  run.bind_buffer<float>("z", z.vector);
  run.bind_arguments(z.shape.x, z.shape.y, 0);

  run.write_buffer("z");

  for (int it = 0; it < iterations; it++)
  {
    run.set_argument(3, it);
    run.execute({z.shape.x, z.shape.y});
  }

  run.read_buffer("z");
}

void thermal_ridge(Array       &z,
                   const Array &talus,
                   int          iterations,
                   Array       *p_deposition_map)
{
  Array z_bckp = Array();
  if (p_deposition_map != nullptr) z_bckp = z;

  auto run = clwrapper::Run("thermal_ridge");

  run.bind_buffer<float>("z", z.vector);
  run.bind_buffer<float>("talus", talus.vector);
  run.bind_arguments(z.shape.x, z.shape.y);

  run.write_buffer("z");
  run.write_buffer("talus");

  for (int it = 0; it < iterations; it++)
    run.execute({z.shape.x, z.shape.y});

  run.read_buffer("z");
  extrapolate_borders(z);

  if (p_deposition_map) *p_deposition_map = abs(z - z_bckp);
}

void thermal_ridge(Array       &z,
                   const Array *p_mask,
                   const Array &talus,
                   int          iterations,
                   Array       *p_deposition_map)
{
  if (!p_mask)
    gpu::thermal_ridge(z, talus, iterations, p_deposition_map);
  else
  {
    Array z_f = z;
    gpu::thermal_ridge(z_f, talus, iterations, p_deposition_map);
    z = lerp(z, z_f, *(p_mask));
  }
}

void thermal_scree(Array       &z,
                   const Array &talus,
                   const Array &zmax,
                   int          iterations,
                   bool         talus_constraint,
                   Array       *p_deposition_map)
{
  Array z_bckp = Array();
  if (p_deposition_map != nullptr) z_bckp = z;

  Array gradient_init = gpu::gradient_norm(z);

  auto run = clwrapper::Run("thermal_scree");

  run.bind_buffer<float>("z", z.vector);
  run.bind_buffer<float>("talus", talus.vector);
  run.bind_buffer<float>("zmax", zmax.vector);
  run.bind_buffer<float>("gradient_init", gradient_init.vector);
  run.bind_arguments(z.shape.x, z.shape.y, talus_constraint ? 1 : 0);

  run.write_buffer("z");
  run.write_buffer("talus");
  run.write_buffer("zmax");

  for (int it = 0; it < iterations; it++)
    run.execute({z.shape.x, z.shape.y});

  run.read_buffer("z");
  extrapolate_borders(z);

  if (p_deposition_map) *p_deposition_map = maximum(z - z_bckp, 0.f);
}

void thermal_scree(Array       &z,
                   const Array *p_mask,
                   const Array &talus,
                   const Array &zmax,
                   int          iterations,
                   bool         talus_constraint,
                   Array       *p_deposition_map)
{
  if (!p_mask)
    gpu::thermal_scree(z,
                       talus,
                       zmax,
                       iterations,
                       talus_constraint,
                       p_deposition_map);
  else
  {
    Array z_f = z;
    gpu::thermal_scree(z_f,
                       talus,
                       zmax,
                       iterations,
                       talus_constraint,
                       p_deposition_map);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap::gpu
