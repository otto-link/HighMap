/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/math.hpp"
#include "highmap/range.hpp"

#include "macrologger.h"

namespace hmap
{

void thermal_rib(Array &z, int iterations, Array *p_bedrock)
{
  Array de = Array(z.shape);

  const std::vector<int>   di = DI;
  const std::vector<int>   dj = DJ;
  const std::vector<float> c = CD;
  const uint               nb = di.size();

  for (int it = 0; it < iterations; it++)
  {
    for (int j = 1; j < z.shape.y - 1; j++)
      for (int i = 1; i < z.shape.x - 1; i++)
      {
        float delta_min = std::numeric_limits<float>::max();
        for (size_t k = 0; k < nb; k++)
        {
          float delta = std::abs(z(i, j) - z(i + di[k], j + dj[k])) / c[k];
          delta_min = std::min(delta_min, delta);
        }
        de(i, j) = delta_min;
      }
    fill_borders(de);
    median_3x3(de);
    z -= de;

    if (p_bedrock) z = maximum(z, *p_bedrock);
  }
}

} // namespace hmap
