/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/array.hpp"
#include "highmap/hydrology.hpp"

namespace hmap
{

void find_flow_sinks(const Array &z, std::vector<int> &is, std::vector<int> &js)
{
  is.clear();
  js.clear();

  const std::vector<int> di = {-1, -1, 0, 1, 1, 1, 0, -1};
  const std::vector<int> dj = {0, 1, 1, 1, 0, -1, -1, -1};
  const uint             nb = di.size();

  for (int j = 1; j < z.shape.y - 1; j++)
    for (int i = 1; i < z.shape.x - 1; i++)
    {
      // count neighbor cells with a higher elevation than the
      // current cell
      int n_higher_cells = 0;
      for (size_t k = 0; k < nb; k++)
      {
        int ik = i + di[k];
        int jk = j + dj[k];
        if (z(i, j) < z(ik, jk)) n_higher_cells++;
      }

      if (n_higher_cells == 8)
      {
        is.push_back(i);
        js.push_back(j);
      }
    }
}

} // namespace hmap
