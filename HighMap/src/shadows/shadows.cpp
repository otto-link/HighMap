/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <numeric>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
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

  for (int j = 0; j < z.shape.y; j++)
    sh(0, j) = z(0, j);

  for (int j = 0; j < z.shape.y; j++)
    for (int i = 1; i < z.shape.x; i++)
      sh(i, j) = std::max(z(i, j), sh(i - 1, j) - shadow_talus);

  sh -= z;
  sh *= -1.f;

  return sh;
}

Array shadow_heightmap(const Array &z,
                       float        azimuth,
                       float        zenith,
                       float        distance)
{
  // https://www.shadertoy.com/view/Xlsfzl
  Array sh = Array(z.shape);
  float azimuth_rad = -M_PI * azimuth / 180.f;
  float zenith_rad = M_PI * zenith / 180.f;
  float vx = distance * std::cos(azimuth_rad) * (float)(z.shape.x - 1);
  float vy = distance * std::sin(azimuth_rad) * (float)(z.shape.y - 1);

  Vec3<float> light_vector = Vec3<float>(vx, vy, std::sin(zenith_rad));

  for (int j = 1; j < z.shape.y - 1; j++)
    for (int i = 1; i < z.shape.x - 1; i++)
    {
      Vec3<float> normal = z.get_normal_at(i, j);
      Vec3<float> pos;

      float ndl = -normal.x * light_vector.x - normal.y * light_vector.y +
                  normal.z * light_vector.z;

      sh(i, j) = 1.f;

      if (ndl > 0.f)
      {
        sh(i, j) = std::max(0.f, sh(i, j) - ndl * 0.6f); // diffuse light

        for (float r = 0.f; r <= 1.f; r += 0.01f)
        {
          pos = {r * light_vector.x + (float)i,
                 r * light_vector.y + (float)j,
                 r * light_vector.z + z(i, j)};

          int ip = (int)pos.x;
          int jp = (int)pos.y;

          if ((ip > 0) and (jp > 0) and (ip < z.shape.x) and (jp < z.shape.y))
          {
            if (pos.z < z(ip, jp) - 0.01f)
            {
              // sh(i, j) = 0.f;
              break;
            }
          }
          else
            break;
        }
      }

      // ambient
      sh(i, j) += std::max(0.f, 0.3f * normal.z);
    }

  fill_borders(sh);

  return sh;
}

} // namespace hmap
