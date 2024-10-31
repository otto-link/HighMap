/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <queue>

#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

Array unwrap_phase(const Array &alpha)
{
  Array    uw_alpha(alpha.shape);
  Mat<int> is_done(alpha.shape);

  // neighbor search
  std::vector<std::tuple<int, int>> directions = {{-1, 0},
                                                  {1, 0},
                                                  {0, -1},
                                                  {0, 1}};

  // queue for flood fill
  std::queue<std::tuple<int, int>> queue;

  // seed point
  int is = 0;
  int js = 0;

  queue.push({is, js});
  uw_alpha(is, js) = alpha(is, js);
  is_done(is, js) = 1;

  // flood fill
  while (!queue.empty())
  {
    auto [i, j] = queue.front();
    queue.pop();

    // visit neighbors
    for (const auto &[di, dj] : directions)
    {
      int ni = i + di;
      int nj = j + dj;

      if (ni >= 0 && ni < alpha.shape.x && nj >= 0 && nj < alpha.shape.y &&
          is_done(ni, nj) == 0)
      {
        float delta_phase = alpha(ni, nj) - alpha(i, j);

        // Adjust for phase discontinuity
        if (delta_phase > M_PI)
          delta_phase -= 2.f * M_PI;
        else if (delta_phase < -M_PI)
          delta_phase += 2.f * M_PI;

        // Set unwrapped phase for the neighbor
        uw_alpha(ni, nj) = uw_alpha(i, j) + delta_phase;
        is_done(ni, nj) = 1;
        queue.push({ni, nj});
      }
    }
  }

  return uw_alpha;
}

} // namespace hmap
