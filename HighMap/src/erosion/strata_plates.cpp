/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <algorithm>
#include <cstdint>

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/math/array.hpp"
#include "highmap/transform.hpp"

namespace hmap::gpu
{

void strata_plates(Array        &z,
                   const Array  &talus,
                   int           direction_offset,
                   int           direction_count,
                   bool          random_directions,
                   std::uint32_t seed,
                   float         vmin,
                   float         skew,
                   float         mix_ratio,
                   const Array  *p_mask,
                   const Array  *p_dx,
                   const Array  *p_dy)
{
  if (!validate_non_empty(z) || !validate_same_shape(z, talus)) return;
  if (p_mask && !validate_same_shape(z, *p_mask)) return;
  if (p_dx && !validate_same_shape(z, *p_dx)) return;
  if (p_dy && !validate_same_shape(z, *p_dy)) return;

  // --- Map parameters to distance_shaper

  float falloff = talus.mean();
  float angle = static_cast<float>(direction_offset % 8) * 45.f;
  float angular_jitter = random_directions ? 0.5f : 0.f;

  z = gpu::distance_shaper(z,
                           falloff,
                  direction_count,
                  skew,
                  angle,
                  hmap::DistanceMode::PYRAMID,
                  hmap::DistanceMethod::RT,
                  angular_jitter,
                  0.f,
                  vmin,
                  false,
                  false,
                  mix_ratio,
                  seed,
                  p_mask,
                  p_dx,
                  p_dy);
}

} // namespace hmap::gpu
