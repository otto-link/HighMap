/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/op.hpp"

#include "macrologger.h"

namespace hmap
{

void hydraulic_ridge(Array &z,
                     float  talus,
                     float  intensity,
                     float  erosion_factor,
                     float  smoothing_factor,
                     float  noise_ratio,
                     int    ir,
                     uint   seed)
{
  // erosion depth
  Array ze;
  if (ir == 0)
    ze = flow_accumulation_dinf(z, talus);
  else
  {
    Array zf = z;
    smooth_cpulse(zf, ir);
    ze = flow_accumulation_dinf(zf, talus);
  }
  ze = log10(ze);
  clamp_max_smooth(ze, erosion_factor, erosion_factor);
  clamp_min(ze, 0.f);
  remap(ze);

  float landing_width_ratio = 0.1f;

  thermal_scree(ze,
                talus,
                seed,
                2.f * erosion_factor,
                0.f, // ze_min
                noise_ratio,
                nullptr,
                smoothing_factor,
                landing_width_ratio,
                false);

  z -= intensity * ze;
}

void hydraulic_ridge(Array &z,
                     float  talus,
                     Array *p_mask,
                     float  intensity,
                     float  erosion_factor,
                     float  smoothing_factor,
                     float  noise_ratio,
                     int    ir,
                     uint   seed)
{
  if (!p_mask)
    hydraulic_ridge(z,
                    talus,
                    intensity,
                    erosion_factor,
                    smoothing_factor,
                    noise_ratio,
                    ir,
                    seed);
  else
  {
    Array z_f = z;
    hydraulic_ridge(z_f,
                    talus,
                    intensity,
                    erosion_factor,
                    smoothing_factor,
                    noise_ratio,
                    ir,
                    seed);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap
