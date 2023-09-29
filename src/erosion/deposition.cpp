/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void sediment_deposition(Array &z,
                         Array &talus,
                         Array *p_deposition_map,
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

    thermal(z_tot, talus, thermal_subiterations, &z);
    smap = z_tot - z;
  }
  z = z + smap;

  if (p_deposition_map)
    *p_deposition_map = smap;
}

void sediment_deposition(Array &z,
                         Array *p_mask,
                         Array &talus,
                         Array *p_deposition_map,
                         float  max_deposition,
                         int    iterations,
                         int    thermal_subiterations)
{
  if (!p_mask)
    sediment_deposition(z,
                        talus,
                        p_deposition_map,
                        max_deposition,
                        iterations,
                        thermal_subiterations);
  else
  {
    Array z_f = z;
    sediment_deposition(z_f,
                        talus,
                        p_deposition_map,
                        max_deposition,
                        iterations,
                        thermal_subiterations);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap
