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

  std::vector<Vec2<int>> nbrs =
      {{-1, 0}, {0, 1}, {0, -1}, {1, 0}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

  float                  zref = z(i, j);
  std::vector<Vec2<int>> queue = {{i, j}};

  // loop around the starting point, anything with elevation lower
  // than the reference elevation is water. If not, the cell is
  // outside the "water" mask
  while (queue.size() > 0)
  {
    Vec2<int> ij = queue.back();
    queue.pop_back();

    for (auto &idx : nbrs)
    {
      Vec2<int> pq = ij + idx;

      if (pq.x >= 0 && pq.x < z.shape.x && pq.y >= 0 && pq.y < z.shape.y)
      {
        float dz = zref - z(pq.x, pq.y);
        if (z(pq.x, pq.y) < zref && dz > fmap(pq.x, pq.y))
        {
          fmap(pq.x, pq.y) = dz;
          queue.push_back({pq.x, pq.y});
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
