#include <cmath>

#include "macrologger.h"
#include "xsimd.hpp"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace xs = xsimd;

namespace hmap
{

void gamma_correction_xsimd(Array &array, float gamma)
{
  using b_type = xs::batch<float>;
  uint   inc = b_type::size;
  uint   size = array.size();
  b_type v_gamma = gamma; // store exponent as a vector

  for (uint i = 0; i < size; i += inc)
  {
    b_type v = b_type::load_unaligned(&(array.vector)[i]);
    v = xs::pow(v, v_gamma);
    v.store_unaligned(&(array.vector)[i]);
  }
}

} // namespace hmap
