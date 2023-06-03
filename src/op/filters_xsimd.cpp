#include <cmath>

#include "Interpolate.hpp"
#include "macrologger.h"
#include "xsimd.hpp"

#include "highmap/array.hpp"

namespace xs = xsimd;

namespace hmap
{

void gamma_correction_xsimd(Array &array, float gamma)
{
  using b_type = xs::batch<float>;
  uint   inc = b_type::size;
  uint   size = array.size();
  b_type v_gamma = gamma; // store exponent as a vector

  for (uint k = 0; k < size; k += inc)
  {
    b_type v = b_type::load_unaligned(&(array.vector)[k]);
    v = xs::pow(v, v_gamma);
    v.store_unaligned(&(array.vector)[k]);
  }
}

} // namespace hmap
