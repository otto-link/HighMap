/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/kernels.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

// --- helpers

int count_neighbors_to_fill(int i, int j, Mat<int> &is_cell_done)
{
  int nb_nbrs = 0;

  for (int p = -1; p < 2; p++)
    for (int q = -1; q < 2; q++)
    {
      int ip = i + p;
      int jq = j + q;
      if (ip >= 0 && ip < is_cell_done.shape.x && jq >= 0 &&
          jq < is_cell_done.shape.y)
        nb_nbrs += is_cell_done(ip, jq);
    }
  return nb_nbrs;
}

bool cmp_queue(std::pair<int, std::pair<int, int>> &a,
               std::pair<int, std::pair<int, int>> &b)
{
  return a.first < b.first;
}

// --- sampling

Array non_parametric_sampling(const Array    &array,
                              hmap::Vec2<int> patch_shape,
                              uint            seed,
                              float           error_threshold)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis;

  hmap::Vec2<int> shape = array.shape;
  Array           array_out = Array(shape);
  Mat<int>        is_cell_done = Mat<int>(shape);

  Array kernel = smooth_cosine(patch_shape);

  // --- initialize output with a small patch in the middle

  Vec2<int> size = Vec2<int>(3, 3);
  int       i1 = (int)(0.5f * shape.x);
  int       j1 = (int)(0.5f * shape.y);

  {
    Array patch = get_random_patch(array, size, gen);

    for (int j = j1; j < j1 + size.y; j++)
      for (int i = i1; i < i1 + size.x; i++)
      {
        array_out(i, j) = patch(i - i1, j - j1);
        is_cell_done(i, j) = 1;
      }
  }

  array_out.infos();

  // --- build queue (number of neighbors, index (i, j))

  std::vector<std::pair<int, std::pair<int, int>>> queue;

  // count number of defined neighbors for each cell (and restrict this
  // search to the initial patch)
  for (int j = j1 - 1; j < j1 + size.y + 1; j++)
    for (int i = i1 - 1; i < i1 + size.x + 1; i++)
      if (is_cell_done(i, j) == 0)
      {
        int nbrs = count_neighbors_to_fill(i, j, is_cell_done);

        if (nbrs > 0) queue.push_back(std::pair(-nbrs, std::pair(i, j)));
      }

  std::make_heap(queue.begin(), queue.end(), cmp_queue);

  // --- synthesis

  while (queue.size() > 0)
  {

    if (queue.size() % 5000 == 0) LOG_DEBUG("queue size: %ld", queue.size());

    std::pair<int, std::pair<int, int>> current = queue.back();
    queue.pop_back();

    int i = current.second.first;
    int j = current.second.second;

    // compute "sum of squared difference" for all possible patches
    std::vector<float> ssd_list = {};
    std::vector<float> value_list = {};

    for (int q = 0; q < shape.y - patch_shape.y; q++)
      for (int p = 0; p < shape.x - patch_shape.x; p++)
      {
        Vec4<int> idx = Vec4<int>(p, p + patch_shape.x, q, q + patch_shape.y);
        Array     patch = array.extract_slice(idx);

        // compute SSD
        int npx2 = (int)std::floor(0.5f * patch_shape.x);
        int npy2 = (int)std::floor(0.5f * patch_shape.y);

        float ssd_sum = 0.f;
        float dsum = 0.f;

        for (int s = 0; s < patch_shape.y; s++)
          for (int r = 0; r < patch_shape.x; r++)
          {
            int ip = i - npx2 + r;
            int jq = j - npy2 + s;

            if (ip >= 0 && ip < shape.x && jq >= 0 && jq < shape.y)
              if (is_cell_done(ip, jq) > 0)
              {
                float v = patch(r, s) - array_out(ip, jq);
                ssd_sum += v * v * kernel(r, s);
                dsum += kernel(r, s);
              }
          }

        if (dsum > 0.f) ssd_sum = ssd_sum / dsum;

        ssd_list.push_back(ssd_sum);
        value_list.push_back(patch(npx2, npy2));
      }

    // pick-up a source patch
    float ssd_best = *std::min_element(ssd_list.begin(), ssd_list.end());
    std::vector<size_t> short_list = {};

    for (size_t k = 0; k < ssd_list.size(); k++)
      if (ssd_list[k] <= ssd_best * (1.f + error_threshold))
        short_list.push_back(k);

    size_t k = (size_t)(dis(gen) * (short_list.size() - 1));
    array_out(i, j) = value_list[short_list[k]];
    is_cell_done(i, j) = 1;

    // add neighbors
    for (int q = -1; q < 2; q++)
      for (int p = -1; p < 2; p++)
      {
        int ip = i + p;
        int jq = j + q;

        if (ip >= 0 && ip < shape.x && jq >= 0 && jq < shape.y)
          if (is_cell_done(ip, jq) == 0)
          {
            int nbrs = count_neighbors_to_fill(ip, jq, is_cell_done);
            if (nbrs > 0)
            {
              queue.push_back(std::pair(-nbrs, std::pair(ip, jq)));
              std::push_heap(queue.begin(), queue.end());
            }
          }
      }
  }

  return array_out;
}

} // namespace hmap
