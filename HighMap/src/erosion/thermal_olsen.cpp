/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <algorithm>

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

#include "macrologger.h"

#define CT 0.5f // avalanching intensity

namespace hmap
{

void thermal_olsen(Array       &z,
                   const Array &talus,
                   int          iterations,
                   Array       *p_bedrock,
                   Array       *p_deposition_map)
{
  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = CD;
  const uint         nb = di.size();

  // keep a backup of the input if the erosion / deposition maps need
  // to be computed
  Array z_bckp = Array();
  if (p_deposition_map != nullptr) z_bckp = z;

  // main loop
  for (int it = 0; it < iterations; it++)
  {
    // modify neighbor search at each iterations to limit numerical
    // artifacts
    std::rotate(di.begin(), di.begin() + 1, di.end());
    std::rotate(dj.begin(), dj.begin() + 1, dj.end());
    std::rotate(c.begin(), c.begin() + 1, c.end());

    if (p_bedrock) // with bedrock
    {
      for (int q = 1; q < z.shape.y - 1; q++)
        for (int p = 1; p < z.shape.x - 1; p++)
        {
          int i, j;

          // alternate row / col order to limit artifacts
          if (it % 4 == 0)
          {
            i = z.shape.x - 1 - p;
            j = q;
          }
          else if (it % 4 == 1)
          {
            i = p;
            j = z.shape.y - 1 - q;
          }
          else if (it % 4 == 2)
          {
            i = z.shape.x - 1 - p;
            j = z.shape.y - 1 - q;
          }
          else
          {
            i = p;
            j = q;
          }

          if (z(i, j) >= (*p_bedrock)(i, j))
          {
            float              dmax = 0.f;
            float              dsum = 0.f;
            std::vector<float> dz(nb);

            for (uint k = 0; k < nb; k++)
            {
              dz[k] = z(i, j) - z(i + di[k], j + dj[k]);
              if (dz[k] > talus(i, j) * c[k])
              {
                dsum += dz[k];
                dmax = std::max(dmax, dz[k]);
              }
            }

            if (dmax > 0.f)
            {
              for (uint k = 0; k < nb; k++)
              {
                int   ia = i + di[k];
                int   ja = j + dj[k];
                float amount = std::min(CT * (dmax - talus(i, j) * c[k]) *
                                            dz[k] / dsum,
                                        z(i, j) - (*p_bedrock)(i, j));
                z(ia, ja) += amount;
              }
            }
          }
        }
    }
    else // no bedrock
    {
      for (int q = 1; q < z.shape.y - 1; q++)
        for (int p = 1; p < z.shape.x - 1; p++)
        {
          int i, j;

          // alternate row / col order to limit artifacts
          if (it % 4 == 0)
          {
            i = z.shape.x - 1 - p;
            j = q;
          }
          else if (it % 4 == 1)
          {
            i = p;
            j = z.shape.y - 1 - q;
          }
          else if (it % 4 == 2)
          {
            i = z.shape.x - 1 - p;
            j = z.shape.y - 1 - q;
          }
          else
          {
            i = p;
            j = q;
          }

          float              dmax = 0.f;
          float              dsum = 0.f;
          std::vector<float> dz(nb);

          for (uint k = 0; k < nb; k++)
          {
            dz[k] = z(i, j) - z(i + di[k], j + dj[k]);
            if (dz[k] > talus(i, j) * c[k])
            {
              dsum += dz[k];
              dmax = std::max(dmax, dz[k]);
            }
          }

          if (dmax > 0.f)
          {
            for (uint k = 0; k < nb; k++)
            {
              int   ia = i + di[k];
              int   ja = j + dj[k];
              float amount = CT * (dmax - talus(i, j) * c[k]) * dz[k] / dsum;
              // amount = std::min(amount, 0.5f * talus(i, j)); // limiter
              z(ia, ja) += amount;
            }
          }
        }
    }
  }

  // clean-up: fix boundaries, remove spurious oscillations and make
  // sure final elevation is not lower than the bedrock
  extrapolate_borders(z);

  if (p_bedrock) clamp_min(z, (*p_bedrock));

  if (p_deposition_map)
  {
    *p_deposition_map = z - z_bckp;
    clamp_min(*p_deposition_map, 0.f);
  }
}

} // namespace hmap
