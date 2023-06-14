#include <algorithm>
#include <list>
#include <numeric>
#include <random>
#include <vector>

#include "macrologger.h"

namespace hmap
{

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

std::vector<size_t> sort_indexes(const std::vector<float> &v)
{
  // https://stackoverflow.com/questions/1577475
  std::vector<size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);
  std::stable_sort(idx.begin(),
                   idx.end(),
                   [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });
  return idx;
}

void reindex_vector(std::vector<int> &v, std::vector<size_t> &idx);
void reindex_vector(std::vector<float> &v, std::vector<size_t> &idx);

} // namespace hmap
