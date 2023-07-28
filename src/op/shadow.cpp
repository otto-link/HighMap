/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#define _USE_MATH_DEFINES
#include <cmath>
#include <numeric>

#include "macrologger.h"

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

Array shadow_heightmap(const Array &z,
                       float        azimuth,
                       float        zenith,
                       float        distance)
{
  // https://www.shadertoy.com/view/Xlsfzl
  Array sh = Array(z.shape);
  float azimuth_rad = -M_PI * azimuth / 180.f;
  float zenith_rad = M_PI * zenith / 180.f;
  float vx = distance * std::cos(azimuth_rad) * (float)(z.shape[0] - 1);
  float vy = distance * std::sin(azimuth_rad) * (float)(z.shape[1] - 1);

  std::vector<float> light_vector = {vx, vy, std::sin(zenith_rad)};

  for (int i = 1; i < z.shape[0] - 1; i++)
    for (int j = 1; j < z.shape[1] - 1; j++)
    {
      std::vector<float> normal = z.get_normal_at(i, j);
      std::vector<float> pos(3);

      float ndl = -normal[0] * light_vector[0] - normal[1] * light_vector[1] +
                  normal[2] * light_vector[2];

      sh(i, j) = 1.f;

      if (ndl > 0.f)
      {
        sh(i, j) = std::max(0.f, sh(i, j) - ndl * 0.6f); // diffuse light

        for (float r = 0.f; r <= 1.f; r += 0.01f)
        {
          pos = {r * light_vector[0] + (float)i,
                 r * light_vector[1] + (float)j,
                 r * light_vector[2] + z(i, j)};

          int ip = (int)pos[0];
          int jp = (int)pos[1];

          if ((ip > 0) and (jp > 0) and (ip < z.shape[0]) and (jp < z.shape[1]))
          {
            if (pos[2] < z(ip, jp) - 0.01f)
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
      sh(i, j) += std::max(0.f, 0.3f * normal[2]);
    }

  fill_borders(sh);

  return sh;
}

} // namespace hmap
