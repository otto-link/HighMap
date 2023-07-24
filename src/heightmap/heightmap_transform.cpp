#include <functional>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/heightmap.hpp"

namespace hmap
{

void fill(HeightMap                                                 &h,
          std::function<Array(std::vector<int>, std::vector<float>)> nullary_op)
{
  LOG_DEBUG("ok");
  for (auto &t : h.tiles)
    t = nullary_op(t.shape, t.shift);
}

void transform1(HeightMap &h, std::function<void(Array &)> unary_op)
{
  LOG_DEBUG("ok");
  for (auto &t : h.tiles)
    unary_op(t);
}

} // namespace hmap
