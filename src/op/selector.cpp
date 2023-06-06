#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

Array select_blob_log(const Array &array, int ir)
{
  Array c = array;
  smooth_cpulse(c, ir);
  c = -laplacian(c);
  return c;
}

Array select_gradient_exp(const Array &array,
                          float        talus_center,
                          float        talus_sigma)
{
  Array c = gradient_norm(array);
  c -= talus_center;
  c = exp(-c * c / (2.f * talus_sigma * talus_sigma));
  return c;
}

Array select_gradient_inv(const Array &array,
                          float        talus_center,
                          float        talus_sigma)
{
  Array c = gradient_norm(array);
  c -= talus_center;
  c = 1.f / (1.f + c * c / (talus_sigma * talus_sigma));
  return c;
}

} // namespace hmap
