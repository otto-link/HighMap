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
#include "highmap/range.hpp"

#include "macrologger.h"

namespace hmap
{

//----------------------------------------------------------------------
// Main operator
//----------------------------------------------------------------------

float helper_thermal_exchange(float self, float other, float dist, float talus)
{
  float max_dif = dist * talus;
  float rate = 0.2f;

  if (self > other)
  {
    if (self - other > max_dif)
      return -rate * ((self - other) - max_dif) / dist;
    else
      return 0.f;
  }
  else
  {
    if (other - self > max_dif)
      return rate * ((other - self) - max_dif) / dist;
    else
      return 0.f;
  }
}

void thermal(Array       &z,
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
            std::vector<float> dz(nb);
            float              amount = 0.f;

            for (int k = 0; k < 8; k++)
              amount += helper_thermal_exchange(z(i, j),
                                                z(i + di[k], j + dj[k]),
                                                c[k],
                                                talus(i, j));

            z(i, j) += amount;
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

          std::vector<float> dz(nb);
          float              amount = 0.f;

          for (int k = 0; k < 8; k++)
            amount += helper_thermal_exchange(z(i, j),
                                              z(i + di[k], j + dj[k]),
                                              c[k],
                                              talus(i, j));

          z(i, j) += amount;
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

//----------------------------------------------------------------------
// Overloading
//----------------------------------------------------------------------

void thermal(Array       &z,
             Array       *p_mask,
             const Array &talus,
             int          iterations,
             Array       *p_bedrock,
             Array       *p_deposition_map)
{
  if (!p_mask)
    thermal(z, talus, iterations, p_bedrock, p_deposition_map);
  else
  {
    Array z_f = z;
    thermal(z_f, talus, iterations, p_bedrock, p_deposition_map);
    z = lerp(z, z_f, *(p_mask));
  }
}

// uniform talus limit, no bedrock
void thermal(Array &z,
             float  talus,
             int    iterations,
             Array *p_bedrock,
             Array *p_deposition_map)
{
  Array talus_map(z.shape, talus);
  thermal(z, talus_map, iterations, p_bedrock, p_deposition_map);
}

//----------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------

void thermal_auto_bedrock(Array       &z,
                          const Array &talus,
                          int          iterations,
                          Array       *p_deposition_map)
{
  Array z_init = z; // backup initial map
  Array bedrock(z.shape, -std::numeric_limits<float>::max());
  int   ncycle = 10;

  Array z_bckp = Array();
  if (p_deposition_map != nullptr) z_bckp = z;

  for (int ic = 0; ic < ncycle; ic++) // thermal weathering cycles
  {
    thermal(z, talus, (int)iterations / ncycle, &bedrock);

    // only keep what's above the initial ground level
    clamp_min(z, z_init);

    // define the bedrock elevation: it is equal to the initial
    // elevation if the elevation after thermal erosion is lower. If
    // not, the bedrock elevation is set to a value smaller than the
    // actual elevation (to allow more erosion).
    std::transform(z_init.vector.begin(),
                   z_init.vector.end(),
                   z.vector.begin(),
                   bedrock.vector.begin(),
                   [](float a, float b)
                   { return a > b ? a : -std::numeric_limits<float>::max(); });
  }

  if (p_deposition_map)
  {
    *p_deposition_map = z - z_bckp;
    clamp_min(*p_deposition_map, 0.f);
  }
}

void thermal_auto_bedrock(Array &z,
                          float  talus,
                          int    iterations,
                          Array *p_deposition_map)
{
  Array talus_map(z.shape, talus);
  thermal_auto_bedrock(z, talus_map, iterations, p_deposition_map);
}

void thermal_auto_bedrock(Array &z,
                          Array *p_mask,
                          float  talus,
                          int    iterations,
                          Array *p_deposition_map)
{
  if (!p_mask)
    thermal_auto_bedrock(z, talus, iterations, p_deposition_map);
  else
  {
    Array z_f = z;
    thermal_auto_bedrock(z_f, talus, iterations, p_deposition_map);
    z = lerp(z, z_f, *(p_mask));
  }
}

} // namespace hmap
