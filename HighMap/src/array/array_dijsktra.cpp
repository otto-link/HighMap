/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <ranges>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap

{

void Array::find_path_dijkstra(Vec2<int>         ij_start,
                               Vec2<int>         ij_end,
                               std::vector<int> &i_path,
                               std::vector<int> &j_path,
                               float             elevation_ratio,
                               float             distance_exponent,
                               Vec2<int>         step,
                               Array            *p_mask_nogo)
{
  // https://math.stackexchange.com/questions/3088292

  Vec2<int> shape_coarse = shape / step;
  Vec2<int> ij_start_coarse = ij_start / step;
  Vec2<int> ij_end_coarse = ij_end / step;

  // neighbors pattern
  const std::vector<int> di = {-1, 0, 0, 1, -1, -1, 1, 1};
  const std::vector<int> dj = {0, 1, -1, 0, -1, 1, -1, 1};
  const size_t           nb = di.size();

  // working queue and arrays
  std::vector<int>   queue_i;
  std::vector<int>   queue_j;
  std::vector<float> queue_d; // distance
  queue_i.reserve(shape_coarse.x * shape_coarse.y);
  queue_j.reserve(shape_coarse.x * shape_coarse.y);
  queue_d.reserve(shape_coarse.x * shape_coarse.y);

  Mat<float> distance(shape_coarse);
  Mat<int>   mask(shape_coarse);
  Mat<int>   next_idx_i(shape_coarse);
  Mat<int>   next_idx_j(shape_coarse);

  // --- Dijkstra's algorithm
  queue_i.push_back(ij_start_coarse.x);
  queue_j.push_back(ij_start_coarse.y);
  queue_d.push_back(0.f);
  mask(ij_start_coarse.x, ij_start_coarse.y) = true;

  while (queue_i.size() > 0)
  {
    // next cell == min distance value
    int kmin = std::distance(queue_d.begin(),
                             std::min_element(queue_d.begin(), queue_d.end()));

    int i = queue_i[kmin];
    int j = queue_j[kmin];
    mask(i, j) = false;

    queue_i.erase(queue_i.begin() + kmin);
    queue_j.erase(queue_j.begin() + kmin);
    queue_d.erase(queue_d.begin() + kmin);

    // LOG_DEBUG("%d %g", kmin, queue_d[kmin]);

    // loop over neighbors
    for (size_t k = 0; k < nb; k++)
    {
      int p = i + di[k];
      int q = j + dj[k];

      if ((p >= 0) and (p < shape_coarse.x) and (q >= 0) and
          (q < shape_coarse.y))
      {
        // previous cumulative value
        float dist = distance(i, j);

        // elevation difference contribution
        dist += (1.f - elevation_ratio) *
                std::pow(std::abs((*this)(i * step.x, j * step.y) -
                                  (*this)(p * step.x, q * step.y)),
                         distance_exponent);

        // aboslute elevation contribution (puts the emphasize on
        // going downslope rather than upslope)
        dist += elevation_ratio * std::max(0.f,
                                           (*this)(p * step.x, q * step.y) -
                                               (*this)(i * step.x, j * step.y));

        if (p_mask_nogo)
          dist += 1e5f * (*p_mask_nogo)(p * step.x, q * step.y);

        if (distance(p, q) == 0.f)
          if ((mask(p, q) == 0) or (dist < distance(p, q)))
          {
            distance(p, q) = dist;

            mask(p, q) = true;
            queue_i.push_back(p);
            queue_j.push_back(q);
            queue_d.push_back(dist);

            next_idx_i(p, q) = i;
            next_idx_j(p, q) = j;
          }
      }
    }
  }

  // --- Build path backwards
  i_path.clear();
  j_path.clear();

  int ic = ij_end_coarse.x;
  int jc = ij_end_coarse.y;

  while ((ic != ij_start_coarse.x) or
         (jc != ij_start_coarse.y)) // TODO add failsafe?
  {
    i_path.push_back(ic);
    j_path.push_back(jc);

    ic = next_idx_i(ic, jc);
    jc = next_idx_j(ic, jc);
  }

  i_path.push_back(ic);
  j_path.push_back(jc);

  for (auto &i : i_path)
    i *= step.x;
  for (auto &j : j_path)
    j *= step.y;

  std::reverse(i_path.begin(), i_path.end());
  std::reverse(j_path.begin(), j_path.end());
}

} // namespace hmap
