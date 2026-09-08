/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <vector>

#include "cl_wrapper/run.hpp"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/math/array.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

void thermal(Array       &z,
             const Array &talus,
             int          iterations,
             const Array *p_bedrock,
             Array       *p_deposition_map)
{
  if (!validate_non_empty(z) || !validate_same_shape(z, talus)) return;
  if (p_bedrock && !validate_same_shape(z, *p_bedrock)) return;

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

void thermal_conserve(Array       &z,
                      const Array &talus,
                      int          iterations,
                      float        rate,
                      const Array *p_bedrock,
                      Array       *p_deposition_map)
{
  if (!validate_non_empty(z) || !validate_same_shape(z, talus)) return;
  if (p_bedrock && !validate_same_shape(z, *p_bedrock)) return;

  Array z_bckp = Array();
  if (p_deposition_map != nullptr) z_bckp = z;

  const glm::ivec2  &shape = z.shape;
  std::vector<float> z_buf = z.vector;

  // run_ab: creates the two 2D images (z_a and z_b)
  auto run_ab = clwrapper::Run("thermal_conserve");

  run_ab.bind_imagef("z_in",
                     z.vector,
                     shape.x,
                     shape.y,
                     clwrapper::Direction::INOUT);
  run_ab.bind_imagef("z_out",
                     z_buf,
                     shape.x,
                     shape.y,
                     clwrapper::Direction::INOUT);
  run_ab.bind_imagef("talus",
                     const_cast<std::vector<float> &>(talus.vector),
                     shape.x,
                     shape.y,
                     clwrapper::Direction::IN);
  run_ab.bind_arguments(shape.x, shape.y, rate);

  // run_ba: shares the same command queue and binds the opposite image handles
  auto run_ba = clwrapper::Run("thermal_conserve", run_ab.get_queue());

  run_ba.bind_image2d("z_in", run_ab.get_image2d("z_out"));
  run_ba.bind_image2d("z_out", run_ab.get_image2d("z_in"));
  run_ba.bind_image2d("talus", run_ab.get_image2d("talus"));
  run_ba.bind_arguments(shape.x, shape.y, rate);

  for (int it = 0; it < iterations; it++)
  {
    if (it % 2 == 0)
      run_ab.execute({shape.x, shape.y});
    else
      run_ba.execute({shape.x, shape.y});
  }

  // read back from the image that received the last write
  if (iterations % 2 == 1)
  {
    run_ab.read_imagef("z_out");
    z.vector = z_buf;
  }
  else
  {
    run_ab.read_imagef("z_in");
  }

  // apply bedrock limit if provided
  if (p_bedrock)
  {
    for (int k = 0; k < z.shape.x * z.shape.y; ++k)
      z.vector[k] = std::max(z.vector[k], p_bedrock->vector[k]);
  }

  if (p_deposition_map) *p_deposition_map = maximum(z - z_bckp, 0.f);
}

void thermal_conserve(Array       &z,
                      const Array *p_mask,
                      const Array &talus,
                      int          iterations,
                      float        rate,
                      const Array *p_bedrock,
                      Array       *p_deposition_map)
{
  apply_with_mask(z,
                  p_mask,
                  [&](Array &a)
                  {
                    gpu::thermal_conserve(a,
                                          talus,
                                          iterations,
                                          rate,
                                          p_bedrock,
                                          p_deposition_map);
                  });
}

void thermal_conserve(Array       &z,
                      float        talus,
                      int          iterations,
                      float        rate,
                      const Array *p_bedrock,
                      Array       *p_deposition_map)
{
  if (!validate_non_empty(z)) return;
  if (p_bedrock && !validate_same_shape(z, *p_bedrock)) return;

  Array talus_map(z.shape, talus);
  gpu::thermal_conserve(z,
                        talus_map,
                        iterations,
                        rate,
                        p_bedrock,
                        p_deposition_map);
}

void thermal(Array       &z,
             const Array *p_mask,
             const Array &talus,
             int          iterations,
             const Array *p_bedrock,
             Array       *p_deposition_map)
{
  apply_with_mask(
      z,
      p_mask,
      [&](Array &a)
      { gpu::thermal(a, talus, iterations, p_bedrock, p_deposition_map); });
}

void thermal(Array       &z,
             float        talus,
             int          iterations,
             const Array *p_bedrock,
             Array       *p_deposition_map)
{
  if (!validate_non_empty(z)) return;
  if (p_bedrock && !validate_same_shape(z, *p_bedrock)) return;

  Array talus_map(z.shape, talus);
  gpu::thermal(z, talus_map, iterations, p_bedrock, p_deposition_map);
}

void thermal_auto_bedrock(Array       &z,
                          const Array &talus,
                          int          iterations,
                          Array       *p_deposition_map)
{
  if (!validate_non_empty(z) || !validate_same_shape(z, talus)) return;

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
  if (!validate_non_empty(z)) return;

  Array talus_map(z.shape, talus);
  gpu::thermal_auto_bedrock(z, talus_map, iterations, p_deposition_map);
}

void thermal_auto_bedrock(Array       &z,
                          const Array *p_mask,
                          const Array &talus,
                          int          iterations,
                          Array       *p_deposition_map)
{
  apply_with_mask(
      z,
      p_mask,
      [&](Array &a)
      { gpu::thermal_auto_bedrock(a, talus, iterations, p_deposition_map); });
}

void thermal_flatten(Array       &z,
                     const Array &talus,
                     int          iterations,
                     float        sigma_inf,
                     float        sigma_sup)
{
  if (!validate_non_empty(z) || !validate_same_shape(z, talus)) return;

  const glm::ivec2 &shape = z.shape;

  auto run = clwrapper::Run("thermal_flatten");

  run.bind_buffer<float>("z", z.vector);
  run.bind_buffer<float>("talus", talus.vector);
  run.bind_arguments(shape.x, shape.y, sigma_inf, sigma_sup);

  run.write_buffer("z");
  run.write_buffer("talus");

  for (int it = 0; it < iterations; it++)
    run.execute({shape.x, shape.y});

  run.read_buffer("z");
  extrapolate_borders(z);
}

void thermal_flatten(Array       &z,
                     const Array *p_mask,
                     const Array &talus,
                     int          iterations,
                     float        sigma_inf,
                     float        sigma_sup)
{
  apply_with_mask(
      z,
      p_mask,
      [&](Array &a)
      { gpu::thermal_flatten(a, talus, iterations, sigma_inf, sigma_sup); });
}

void thermal_inflate(Array &z, const Array &talus, int iterations)
{
  if (!validate_non_empty(z) || !validate_same_shape(z, talus)) return;

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
  apply_with_mask(z,
                  p_mask,
                  [&](Array &a)
                  { gpu::thermal_inflate(a, talus, iterations); });
}

void thermal_olsen(Array &z, const Array &talus, int iterations)
{
  if (!validate_non_empty(z) || !validate_same_shape(z, talus)) return;

  auto run = clwrapper::Run("thermal_olsen");

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

void thermal_olsen(Array       &z,
                   const Array *p_mask,
                   const Array &talus,
                   int          iterations)
{
  apply_with_mask(z,
                  p_mask,
                  [&](Array &a) { gpu::thermal_olsen(a, talus, iterations); });
}

void thermal_rib(Array &z, int iterations)
{
  if (!validate_non_empty(z)) return;

  auto run = clwrapper::Run("thermal_rib");

  run.bind_buffer<float>("z", z.vector);
  run.bind_arguments(z.shape.x, z.shape.y);

  run.write_buffer("z");

  for (int it = 0; it < iterations; it++)
  {
    run.execute({z.shape.x, z.shape.y});
  }

  run.read_buffer("z");
  extrapolate_borders(z, 3);
}

void thermal_rib(Array &z, const Array *p_mask, int iterations)
{
  apply_with_mask(z,
                  p_mask,
                  [&](Array &a) { gpu::thermal_rib(a, iterations); });
}

void thermal_ridge(Array       &z,
                   const Array &talus,
                   int          iterations,
                   Array       *p_deposition_map)
{
  if (!validate_non_empty(z) || !validate_same_shape(z, talus)) return;

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
  apply_with_mask(z,
                  p_mask,
                  [&](Array &a) {
                    gpu::thermal_ridge(a, talus, iterations, p_deposition_map);
                  });
}

void thermal_schott(Array       &z,
                    const Array &talus,
                    int          iterations,
                    float        intensity,
                    Array       *p_deposition_map)
{
  if (!validate_non_empty(z) || !validate_same_shape(z, talus)) return;

  Array z_bckp = Array();
  if (p_deposition_map != nullptr) z_bckp = z;

  auto run = clwrapper::Run("thermal_schott");

  run.bind_buffer<float>("z", z.vector);
  run.bind_buffer<float>("talus", talus.vector);
  run.bind_arguments(z.shape.x, z.shape.y, intensity);

  run.write_buffer("z");
  run.write_buffer("talus");

  for (int it = 0; it < iterations; it++)
    run.execute({z.shape.x, z.shape.y});

  run.read_buffer("z");
  extrapolate_borders(z);

  if (p_deposition_map) *p_deposition_map = abs(z - z_bckp);
}

void thermal_schott(Array       &z,
                    const Array *p_mask,
                    const Array &talus,
                    int          iterations,
                    float        intensity,
                    Array       *p_deposition_map)
{
  apply_with_mask(
      z,
      p_mask,
      [&](Array &a) {
        gpu::thermal_schott(a, talus, iterations, intensity, p_deposition_map);
      });
}

void thermal_scree(Array       &z,
                   const Array &talus,
                   const Array &zmax,
                   int          iterations,
                   Array       *p_deposition_map)
{
  if (!validate_non_empty(z) || !validate_same_shape(z, talus) ||
      !validate_same_shape(z, zmax))
    return;

  Array z_bckp = Array();
  if (p_deposition_map != nullptr) z_bckp = z;

  auto run = clwrapper::Run("thermal_scree");

  run.bind_buffer<float>("z", z.vector);
  run.bind_buffer<float>("talus", talus.vector);
  run.bind_buffer<float>("zmax", zmax.vector);
  run.bind_arguments(z.shape.x, z.shape.y);

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
                   Array       *p_deposition_map)
{
  apply_with_mask(
      z,
      p_mask,
      [&](Array &a)
      { gpu::thermal_scree(a, talus, zmax, iterations, p_deposition_map); });
}

} // namespace hmap::gpu
