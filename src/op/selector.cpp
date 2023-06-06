#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

Array select_gradient_norm_exp(const Array &array, float talus)
{
  Array c = gradient_norm(array);
  c = exp(-c * c / (2.f * talus * talus));
  return c;
}

Array select_gradient_norm_inv(const Array &array, float talus)
{
  Array c = gradient_norm(array);
  c = 1.f / (1.f + c * c / (talus * talus));
  return c;
}

} // namespace hmap
