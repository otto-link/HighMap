/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

namespace hmap
{

void erosion_maps(Array &z_before,
                  Array &z_after,
                  Array &erosion_map,
                  Array &deposition_map,
                  float  tolerance)
{
  erosion_map = z_before - z_after + tolerance;
  clamp_min(erosion_map, 0.f);

  deposition_map = z_after - z_before + tolerance;
  clamp_min(deposition_map, 0.f);
}

} // namespace hmap
