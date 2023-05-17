#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

Array blend_overlay(const Array &array1, const Array &array2)
{
  Array array_out = Array(array1.shape);
  auto  lambda = [](float a, float b)
  { return a < 0.5 ? 2.f * a * b : 1.f - 2.f * (1.f - a) * (1.f - b); };

  std::transform(array1.vector.begin(),
                 array1.vector.end(),
                 array2.vector.begin(),
                 array_out.vector.begin(),
                 lambda);

  return array_out;
}

Array blend_soft(const Array &array1, const Array &array2)
{
  Array array_out = Array(array1.shape);
  array_out = (1.f - array1) * array1 * array2 +
              array1 * (1.f - (1.f - array1) * (1.f - array2));
  return array_out;
}

} // namespace hmap
