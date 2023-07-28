/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"

namespace hmap
{

void sediment_deposition(Array &z,
                         Array &talus,
                         float  max_deposition,
                         int    iterations,
                         int    thermal_subiterations)
{
  float deposition_step = 0.5f * max_deposition;
  Array smap = Array(z.shape); // sediment map

  for (int it = 0; it < iterations; it++)
  {
    smap = smap + deposition_step;
    Array z_tot = z + smap;

    thermal(z_tot,
            talus,
            z, // bedrock
            thermal_subiterations);
    smap = z_tot - z;
  }
  z = z + smap;
}

} // namespace hmap