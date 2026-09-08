/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <cmath>
#include <vector>

#include "cl_wrapper/run.hpp"

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/gradient.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/logger.hpp"

// #include "highmap/boundary.hpp"
// #include "highmap/hydrology/hydrology.hpp"
// #include "highmap/math/core.hpp"

#include "highmap/filters.hpp"
#include "highmap/range.hpp"

namespace hmap::gpu
{

// --- Helper functions

namespace
{

float helper_compute_snow_talus_slope(float snow_depth,
                                      float snow_density,
                                      float snow_cohesion,
                                      float snow_friction_talus,
                                      float gravity)
{
  if (snow_depth <= 0.0f) return std::numeric_limits<float>::infinity();

  const float rho_g_s = snow_density * gravity * snow_depth;

  // Cohesionless limit:
  // rho*g*S*s > rho*g*S*snow_friction_talus  =>  s > snow_friction_talus
  if (snow_cohesion <= 1e-6f) return snow_friction_talus;

  const float discriminant = rho_g_s * rho_g_s -
                             4.0f * snow_cohesion *
                                 (snow_cohesion +
                                  rho_g_s * snow_friction_talus);

  // No unstable solution for this snow state.
  if (discriminant <= 0.0f) return std::numeric_limits<float>::infinity();

  const float sqrt_discriminant = std::sqrt(discriminant);

  // First/root corresponding to the onset of instability.
  const float critical_slope = (rho_g_s - sqrt_discriminant) /
                               (2.0f * snow_cohesion);

  return critical_slope;
}

} // namespace

// --- Main API

Array snow_simulation_layered(const Array &z,
                              float        snow_depth,
                              const Array &talus)
{
  if (!validate_non_empty(z)) return Array();
  if (!validate_same_shape(z, talus)) return Array();

  const glm::ivec2 shape = z.shape;
  const int        n_cells = shape.x * shape.y;

  // parameters
  const int   iterations = 20;
  const int   thermal_sub_iterations = 200;
  const float snow_friction_talus = 0.6f / shape.x;
  const float snow_density = 1.0f;
  const float snow_cohesion = 0.05f;
  const float gravity = 1.0f;
  const float critical_slope_max =
      1.2f / shape.x; // max slope angle for fresh snow adhesion

  const float snow_layer_depth = snow_depth / float(iterations);

  // compute ground slope (gradient norm) to modulate initial deposition
  Array slope = 0.5f * gpu::gradient_norm(z);

  // snow accumulation field
  Array s(shape, 0.f);

  for (int it = 0; it < iterations; ++it)
  {
    // add fresh snow layer with slope-dependent adhesion
    for (int k = 0; k < n_cells; ++k)
    {
      // fresh snow has difficulty sticking to slopes steeper than
      // critical_slope_max
      float sl = slope.vector[k];
      float adhesion = 1.0f - std::clamp((sl - 0.4f * critical_slope_max) /
                                             (0.6f * critical_slope_max),
                                         0.0f,
                                         0.9f);
      s.vector[k] += snow_layer_depth * adhesion;
    }

    // surface elevation: bedrock + snow
    Array zs = z + s;

    // smooth the snow surface gently to simulate wind drift / settling
    if (it > 0)
    {
      gpu::smooth_cpulse(zs, 4);
      zs = maximum(zs, z);
      s = zs - z;
    }

    // compute effective talus limit based on current snow depth and bedrock
    // talus
    Array talus_effective(shape);
    for (int k = 0; k < n_cells; ++k)
    {
      const float current_depth = s.vector[k];
      const float snow_talus = helper_compute_snow_talus_slope(
          current_depth,
          snow_density,
          snow_cohesion,
          snow_friction_talus,
          gravity);

      talus_effective.vector[k] = std::min(talus.vector[k], snow_talus);
    }

    // conservative thermal erosion/avalanche transport over the combined
    // surface
    gpu::thermal_conserve(zs,
                          talus_effective,
                          thermal_sub_iterations,
                          0.4f,
                          &z);

    // update snow depth
    s = zs - z;
    clamp_min(s, 0.f);
  }

  // final settling pass on snow cover
  clamp_min(s, 0.f);

  return s;
}

} // namespace hmap::gpu
