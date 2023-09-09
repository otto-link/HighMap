/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <algorithm>
#include <list>
#include <numeric>
#include <random>
#include <vector>

#include "macrologger.h"

namespace hmap
{

std::vector<size_t> argsort(const std::vector<float> &v)
{
  // https://stackoverflow.com/questions/1577475
  std::vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);
  std::stable_sort(idx.begin(),
                   idx.end(),
                   [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });
  return idx;
}

size_t upperbound_right(const std::vector<float> &v, float value)
{
  size_t idx = 0;
  for (size_t k = v.size() - 1; k > 0; k--)
    if (value > v[k])
    {
      idx = k;
      break;
    }
  return idx;
}

void reindex_vector(std::vector<int> &v, std::vector<size_t> &idx);
void reindex_vector(std::vector<float> &v, std::vector<size_t> &idx);

void vector_unique_values(std::vector<float> &v)
{
  auto last = std::unique(v.begin(), v.end());
  v.erase(last, v.end());
  std::sort(v.begin(), v.end());
  last = std::unique(v.begin(), v.end());
  v.erase(last, v.end());
}

} // namespace hmap
