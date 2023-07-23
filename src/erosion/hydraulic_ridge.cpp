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
                     uint   seed)
{
  // erosion depth
  Array ze = flow_accumulation_dinf(z, talus);
  ze = log10(ze);
  clamp_max_smooth(ze, erosion_factor, erosion_factor);
  remap(ze);

  float landing_width_ratio = 0.1f;

  thermal_scree(ze,
                talus,
                seed,
                0.f, // ze_min
                2.f * erosion_factor,
                noise_ratio,
                smoothing_factor,
                landing_width_ratio,
                false);

  z -= intensity * ze;
}

} // namespace hmap
