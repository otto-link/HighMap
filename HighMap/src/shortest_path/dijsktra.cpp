/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/math.hpp"

namespace hmap
{

void find_path_dijkstra(const Array                   &z,
                        Vec2<int>                      ij_start,
                        std::vector<Vec2<int>>         ij_end_list,
                        std::vector<std::vector<int>> &i_path_list,
                        std::vector<std::vector<int>> &j_path_list,
                        float                          elevation_ratio,
                        float                          distance_exponent,
                        float                          upward_penalization,
                        const Array                   *p_mask_nogo)
{
  // https://math.stackexchange.com/questions/3088292

  Vec2<int> shape = z.shape;

  // neighbors pattern
  const std::vector<int>   di = {-1, 0, 0, 1, -1, -1, 1, 1};
  const std::vector<int>   dj = {0, 1, -1, 0, -1, 1, -1, 1};
  const std::vector<float> cd =
      {1.f, 1.f, 1.f, 1.f, M_SQRT2, M_SQRT2, M_SQRT2, M_SQRT2};
  const size_t nb = di.size();

  // working queue and arrays
  std::vector<int>   queue_i;
  std::vector<int>   queue_j;
  std::vector<float> queue_d; // distance
  queue_i.reserve(shape.x * shape.y);
  queue_j.reserve(shape.x * shape.y);
  queue_d.reserve(shape.x * shape.y);

  Mat<float> distance(shape);
  Mat<int>   mask(shape);
  Mat<int>   next_idx_i(shape);
  Mat<int>   next_idx_j(shape);

  // --- Dijkstra's algorithm

  queue_i.push_back(ij_start.x);
  queue_j.push_back(ij_start.y);
  queue_d.push_back(0.f);
  mask(ij_start.x, ij_start.y) = true;

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

    // loop over neighbors
    for (size_t k = 0; k < nb; k++)
    {
      int p = i + di[k];
      int q = j + dj[k];

      if ((p >= 0) and (p < shape.x) and (q >= 0) and (q < shape.y))
      {
        // previous cumulative value
        float dist = distance(i, j);

        // elevation difference contribution (weighted for diagonal
        // directions to avoid artifacts)
        float dz = (z(i, j) - z(p, q)) * cd[k];
        if (dz < 0.f) dz *= upward_penalization;
        dz = std::abs(dz);

        dist += (1.f - elevation_ratio) * std::pow(dz, distance_exponent);

        // absolute elevation contribution (puts the emphasize on
        // going downslope rather than upslope)
        dist += elevation_ratio * std::max(0.f, cd[k] * (z(p, q) - z(i, j)));

        if (p_mask_nogo) dist += 1e5f * (*p_mask_nogo)(p, q);

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

  i_path_list.clear();
  j_path_list.clear();

  for (auto ij_end : ij_end_list)
  {
    int ic = ij_end.x;
    int jc = ij_end.y;

    std::vector<int> i_path, j_path;

    while ((ic != ij_start.x) or (jc != ij_start.y))
    {
      i_path.push_back(ic);
      j_path.push_back(jc);

      ic = next_idx_i(ic, jc);
      jc = next_idx_j(ic, jc);
    }

    i_path.push_back(ic);
    j_path.push_back(jc);

    std::reverse(i_path.begin(), i_path.end());
    std::reverse(j_path.begin(), j_path.end());

    i_path_list.push_back(i_path);
    j_path_list.push_back(j_path);
  }
}

void find_path_dijkstra(const Array      &z,
                        Vec2<int>         ij_start,
                        Vec2<int>         ij_end,
                        std::vector<int> &i_path,
                        std::vector<int> &j_path,
                        float             elevation_ratio,
                        float             distance_exponent,
                        float             upward_penalization,
                        const Array      *p_mask_nogo)
{
  std::vector<std::vector<int>> i_path_list = {i_path};
  std::vector<std::vector<int>> j_path_list = {j_path};

  find_path_dijkstra(z,
                     ij_start,
                     {ij_end},
                     i_path_list,
                     j_path_list,
                     elevation_ratio,
                     distance_exponent,
                     upward_penalization,
                     p_mask_nogo);

  i_path = i_path_list[0];
  j_path = j_path_list[0];
}

} // namespace hmap
