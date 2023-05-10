#include <vector>

#include "highmap/array.hpp"

namespace hmap
{

hmap::Array constant(std::vector<int> shape, float value)
{
  hmap::Array array = hmap::Array(shape);
  for (auto &v : array.vector)
    v = value;
  return array;
}

} // namespace hmap
