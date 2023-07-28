/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

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

void make_binary_xsimd(Array &array, float threshold)
{
  using b_type = xs::batch<float>;
  uint   inc = b_type::size;
  uint   size = array.size();
  b_type v_threshold = threshold;
  b_type v_ones = 1.f;
  b_type v_zeros = 0.f;

  for (uint k = 0; k < size; k += inc)
  {
    b_type v = b_type::load_unaligned(&(array.vector)[k]);
    v = xs::select(xs::gt(v, v_threshold), v_ones, v_zeros);
    v.store_unaligned(&(array.vector)[k]);
  }
}

} // namespace hmap
