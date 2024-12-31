/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/math.hpp"

// neighbor pattern search
// 6 2 8
// 1 . 4
// 5 3 7
// clang-format off
#define DI {-1, 0, 0, 1, -1, -1, 1, 1}
#define DJ {0, 1, -1, 0, -1, 1, -1, 1}
#define C  {1.f, 1.f, 1.f, 1.f, M_SQRT2, M_SQRT2, M_SQRT2, M_SQRT2}
// clang-format on

namespace hmap
{

void depression_filling(Array &z, int iterations, float epsilon)
{
  std::vector<int>   di = DI;
  std::vector<int>   dj = DJ;
  std::vector<float> c = C;
  const uint         nb = di.size();

  Array z_new = z;
  z_new.set_slice({1, z.shape.x - 1, 1, z.shape.y - 1}, 1e6f);

  for (int it = 0; it < iterations; it++)
  {
    for (int j = 1; j < z.shape.y - 1; j++)
      for (int i = 1; i < z.shape.x - 1; i++)
      {
        if (z_new(i, j) > z(i, j))
        {
          for (uint k = 0; k < nb; k++)
          {
            int p = i + di[k];
            int q = j + dj[k];

            if (z(i, j) >= z_new(p, q) + epsilon * c[k])
            {
              z_new(i, j) = z(i, j);
              break;
            }

            if (z_new(i, j) > z_new(p, q) + epsilon * c[k])
              z_new(i, j) = z_new(p, q) + epsilon * c[k];
          }
        }
      }
  }
  extrapolate_borders(z_new);
  z = z_new;
}

} // namespace hmap
