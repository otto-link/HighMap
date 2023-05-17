#define _USE_MATH_DEFINES

#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array hillshade(Array &z, float azimuth, float zenith, float talus_ref)
{
  float azimuth_rad = M_PI * azimuth / 180.f;
  float zenith_rad = M_PI * zenith / 180.f;

  Array aspect = gradient_angle(z, true);
  Array dn = gradient_norm(z) / talus_ref;
  Array slope = atan(dn);

  Array sh = std::cos(zenith_rad) * cos(slope) +
             std::sin(zenith_rad) * sin(slope) * cos(azimuth_rad - aspect);

  return sh;
}

Array topographic_shading(Array &z,
                          float  azimuth,
                          float  zenith,
                          float  talus_ref)
{
  const float azimuth_rad = M_PI * azimuth / 180.f;
  const float zenith_rad = M_PI * zenith / 180.f;

  Array aspect = gradient_angle(z, true);
  Array dn = gradient_norm(z) / talus_ref;
  Array slope = atan(dn);

  Array sh = cos(slope - zenith_rad) * cos(azimuth_rad - aspect);

  return sh;
}

} // namespace hmap
