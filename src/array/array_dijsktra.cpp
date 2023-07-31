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

void Array::find_path_dijkstra(std::vector<int>  ij_start,
                               std::vector<int>  ij_end,
                               std::vector<int> &i_path,
                               std::vector<int> &j_path,
                               float             distance_exponent,
                               std::vector<int>  step)
{
  // https://math.stackexchange.com/questions/3088292

  std::vector<int> shape_coarse = {this->shape[0] / step[0],
                                   this->shape[1] / step[1]};

  std::vector<int> ij_start_coarse = {ij_start[0] / step[0],
                                      ij_start[1] / step[1]};

  std::vector<int> ij_end_coarse = {ij_end[0] / step[0], ij_end[1] / step[1]};

  // neighbors pattern
  const std::vector<int> di = {-1, 0, 0, 1, -1, -1, 1, 1};
  const std::vector<int> dj = {0, 1, -1, 0, -1, 1, -1, 1};
  const size_t           nb = di.size();

  // working queue and arrays
  std::vector<int>   queue_i;
  std::vector<int>   queue_j;
  std::vector<float> queue_d; // distance
  queue_i.reserve(shape_coarse[0] * shape_coarse[1]);
  queue_j.reserve(shape_coarse[0] * shape_coarse[1]);
  queue_d.reserve(shape_coarse[0] * shape_coarse[1]);

  std::vector<float> distance(shape_coarse[0] * shape_coarse[1]);
  std::vector<bool>  mask(shape_coarse[0] * shape_coarse[1]);
  std::vector<int>   next_idx_i(shape_coarse[0] * shape_coarse[1]);
  std::vector<int>   next_idx_j(shape_coarse[0] * shape_coarse[1]);

  // --- Dijkstra's algorithm
  queue_i.push_back(ij_start_coarse[0]);
  queue_j.push_back(ij_start_coarse[1]);
  queue_d.push_back(0.f);
  mask[ij_start_coarse[0] * shape_coarse[1] + ij_start_coarse[1]] = true;

  while (queue_i.size() > 0)
  {
    // next cell == min distance value
    int kmin = std::distance(queue_d.begin(),
                             std::min_element(queue_d.begin(), queue_d.end()));

    int i = queue_i[kmin];
    int j = queue_j[kmin];
    mask[i * shape_coarse[1] + j] = false;

    queue_i.erase(queue_i.begin() + kmin);
    queue_j.erase(queue_j.begin() + kmin);
    queue_d.erase(queue_d.begin() + kmin);

    // loop over neighbors
    for (size_t k = 0; k < nb; k++)
    {
      int p = i + di[k];
      int q = j + dj[k];

      if ((p > 0) and (p < shape_coarse[0]) and (q > 0) and
          (q < shape_coarse[1]))
      {
        float dist = distance[i * shape_coarse[1] + j] +
                     std::pow(std::abs((*this)(i * step[0], j * step[1]) -
                                       (*this)(p * step[0], q * step[1])),
                              distance_exponent);

        if (distance[p * shape_coarse[1] + q] == 0.f)
          if ((mask[p * shape_coarse[1] + q] == false) or
              (dist < distance[p * shape_coarse[1] + q]))
          {
            distance[p * shape_coarse[1] + q] = dist;

            mask[p * shape_coarse[1] + q] = true;
            queue_i.push_back(p);
            queue_j.push_back(q);
            queue_d.push_back(dist);

            next_idx_i[p * shape_coarse[1] + q] = i;
            next_idx_j[p * shape_coarse[1] + q] = j;
          }
      }
    }
  }

  // --- Build path backwards
  i_path.clear();
  j_path.clear();

  int ic = ij_end_coarse[0];
  int jc = ij_end_coarse[1];

  while ((ic != ij_start_coarse[0]) or
         (jc != ij_start_coarse[1])) // TODO add failsafe?
  {
    i_path.push_back(ic);
    j_path.push_back(jc);

    int k = ic * shape_coarse[1] + jc;
    ic = next_idx_i[k];
    jc = next_idx_j[k];
  }

  i_path.push_back(ic);
  j_path.push_back(jc);

  for (auto &i : i_path)
    i *= step[0];
  for (auto &j : j_path)
    j *= step[1];

  std::reverse(i_path.begin(), i_path.end());
  std::reverse(j_path.begin(), j_path.end());
}

} // namespace hmap
