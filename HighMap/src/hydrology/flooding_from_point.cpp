/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/range.hpp"

namespace hmap
{

Array flooding_from_point(const Array &z, const int i, const int j)
{
  Array fmap(z.shape, 0.f);

  std::vector<std::pair<int, int>> nbrs =
      {{-1, 0}, {0, 1}, {0, -1}, {1, 0}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

  float                            zref = z(i, j);
  std::vector<std::pair<int, int>> queue = {{i, j}};

  // loop around the starting point, anything with elevation lower
  // than the reference elevation is water. If not, the cell is
  // outside the "water" mask
  while (queue.size() > 0)
  {
    std::pair<int, int> ij = queue.back();
    queue.pop_back();

    for (auto &idx : nbrs)
    {
      int p = ij.first + idx.first;
      int q = ij.second + idx.second;

      if (p >= 0 && p < z.shape.x && q >= 0 && q < z.shape.y)
      {
        float dz = zref - z(p, q);
        if (z(p, q) < zref && dz > fmap(p, q))
        {
          fmap(p, q) = dz;
          queue.push_back({p, q});
        }
      }
    }
  }

  return fmap;
}

Array flooding_from_point(const Array            &z,
                          const std::vector<int> &i,
                          const std::vector<int> &j)
{
  Array fmap(z.shape);

  for (size_t k = 0; k < i.size(); k++)
    fmap = maximum(fmap, flooding_from_point(z, i[k], j[k]));

  return fmap;
}

} // namespace hmap
