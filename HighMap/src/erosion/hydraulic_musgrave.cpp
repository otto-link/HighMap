/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/filters.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

// neighbor pattern search
// 5 6 7
// 4 . 0
// 3 2 1
// clang-format off
#define DI {-1, 0, 0, 1, -1, -1, 1, 1}
#define DJ {0, 1, -1, 0, -1, 1, -1, 1}
#define C  {1.f, 1.f, 1.f, 1.f, M_SQRT1_2, M_SQRT1_2, M_SQRT1_2, M_SQRT1_2}
// clang-format on
#define LAPLACE_PERIOD 10
#define LAPLACE_SIGMA 0.05f
#define LAPLACE_ITERATIONS 1

//----------------------------------------------------------------------
// Main operator
//----------------------------------------------------------------------

void hydraulic_musgrave(Array &z,
                        Array &moisture_map,
                        int    iterations,
                        float  c_capacity,
                        float  c_erosion,
                        float  c_deposition,
                        float  water_level,
                        float  evap_rate)
{
  Array s = constant(z.shape);          // sediment level
  Array w = water_level * moisture_map; // backup initial moisture map

  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = C;
  const uint         nb = di.size();

  for (int it = 0; it < iterations; it++)
  {
    w = (1 - evap_rate) * w + evap_rate * moisture_map * water_level;

    // modify neighbor search at each iterations to limit numerical
    // artifacts
    std::rotate(di.begin(), di.begin() + 1, di.end());
    std::rotate(dj.begin(), dj.begin() + 1, dj.end());
    std::rotate(c.begin(), c.begin() + 1, c.end());

    for (int j = 1; j < z.shape.y - 1; j++)
      for (int i = 1; i < z.shape.x - 1; i++)
        for (uint k = 0; k < nb; k++) // loop over 1st neighbors
        {
          int   p = i + di[k];
          int   q = j + dj[k];
          float dw = std::min(w(i, j),
                              (w(i, j) + z(i, j) - w(p, q) - z(p, q)) * c[k]);

          if (dw <= 0.f)
          {
            // sediment deposition
            z(i, j) = z(i, j) + c_deposition * s(i, j);
            s(i, j) = (1.f - c_deposition) * s(i, j);
          }
          else
          {
            w(i, j) = w(i, j) - 0.5f * dw;
            w(p, q) = w(p, q) + 0.5f * dw;

            // differential of sediment capacity of water gap (ks *
            // dw)
            float sc = c_capacity * dw;
            float delta_sc = s(i, j) - sc;
            if (delta_sc > 0.f)
            {
              // deposition
              s(p, q) = s(p, q) + sc;
              z(i, j) = z(i, j) + c_deposition * delta_sc;
              s(i, j) = (1.f - c_deposition) * delta_sc;
            }
            else
            {
              // erosion
              s(p, q) = s(p, q) + s(i, j) - c_erosion * delta_sc;
              z(i, j) = z(i, j) + c_erosion * delta_sc;
              s(i, j) = 0.f;
            }
          }
        } // k

    // fix boundaries
    fill_borders(z);
    fill_borders(w);
    fill_borders(s);

    // apply low-pass filtering to smooth spurious spatial
    // oscillations
    if (it % LAPLACE_PERIOD == 0) laplace(z, LAPLACE_SIGMA, LAPLACE_ITERATIONS);

  } // it

  extrapolate_borders(z);
  laplace(z, LAPLACE_SIGMA, LAPLACE_ITERATIONS);
}

//----------------------------------------------------------------------
// Overloading
//----------------------------------------------------------------------

// uniform moisture map
void hydraulic_musgrave(Array &z,
                        int    iterations,
                        float  c_capacity,
                        float  c_erosion,
                        float  c_deposition,
                        float  water_level,
                        float  evap_rate)
{
  Array mmap = constant(z.shape, 1.f);
  hydraulic_musgrave(z,
                     mmap,
                     iterations,
                     c_capacity,
                     c_erosion,
                     c_deposition,
                     water_level,
                     evap_rate);
}

} // namespace hmap
