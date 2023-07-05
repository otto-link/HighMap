#define _USE_MATH_DEFINES

#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array hillshade(const Array &z, float azimuth, float zenith, float talus_ref)
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

Array topographic_shading(const Array &z,
                          float        azimuth,
                          float        zenith,
                          float        talus_ref)
{
  const float azimuth_rad = M_PI * azimuth / 180.f;
  const float zenith_rad = M_PI * zenith / 180.f;

  Array aspect = gradient_angle(z, true);
  Array dn = gradient_norm(z) / talus_ref;
  Array slope = atan(dn);

  Array sh = cos(slope - zenith_rad) * cos(azimuth_rad - aspect);

  return sh;
}

Array shadow_grid(const Array &z, float shadow_talus)
{
  Array sh = Array(z.shape);

  for (int j = 0; j < z.shape[1]; j++)
    sh(0, j) = z(0, j);

  for (int i = 1; i < z.shape[0]; i++)
    for (int j = 0; j < z.shape[1]; j++)
      sh(i, j) = std::max(z(i, j), sh(i - 1, j) - shadow_talus);

  sh -= z;
  sh *= -1.f;

  return sh;
}

} // namespace hmap
