/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/morphology.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

#include "macrologger.h"

namespace hmap
{

//----------------------------------------------------------------------
// Main operator
//----------------------------------------------------------------------

void thermal_flatten(Array       &z,
                     const Array &talus,
                     const Array &bedrock,
                     int          iterations,
                     int          post_filter_ir)
{
  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = CD;
  const uint         nb = di.size();

  Array z_bckp = z;

  // main loop
  for (int it = 0; it < iterations; it++)
  {
    // modify neighbor search at each iterations to limit numerical
    // artifacts
    std::rotate(di.begin(), di.begin() + 1, di.end());
    std::rotate(dj.begin(), dj.begin() + 1, dj.end());
    std::rotate(c.begin(), c.begin() + 1, c.end());

    for (int j = 1; j < z.shape.y - 1; j++)
      for (int i = 1; i < z.shape.x - 1; i++)
      {
        if (z(i, j) > bedrock(i, j))
        {
          float dmax = 0.f;
          int   ka = -1;

          for (uint k = 0; k < nb; k++)
          {
            float dz = (z(i, j) - z(i + di[k], j + dj[k])) / c[k];
            if (dz > dmax)
            {
              dmax = dz;
              ka = k;
            }
          }

          if (dmax > 0.f and dmax < talus(i, j))
          {
            float amount = 0.5f * dmax;
            z(i, j) -= amount;
            z(i + di[ka], j + dj[ka]) += amount;
          }
        }
      }
  }

  // clean-up: fix boundaries
  extrapolate_borders(z);

  // remove spurious oscillations
  smooth_cpulse(z, post_filter_ir);
  z = maximum_smooth(z, z_bckp, 0.01f);

  // make sure final elevation is not lower than the bedrock
  clamp_min(z, bedrock);
}

//----------------------------------------------------------------------
// Overloading
//----------------------------------------------------------------------

void thermal_flatten(Array &z, float talus, int iterations, int post_filter_ir)
{
  Array talus_map = constant(z.shape, talus);
  Array bedrock = constant(z.shape, z.min() - z.ptp());
  thermal_flatten(z, talus_map, bedrock, iterations, post_filter_ir);
}

} // namespace hmap
