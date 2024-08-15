/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/convolve.hpp"
#include "highmap/kernels.hpp"
#include "highmap/primitives.hpp"

#define SVD_RANK 2

namespace hmap
{

Array gabor_noise(Vec2<int> shape,
                  float     kw,
                  float     angle,
                  int       width,
                  float     density,
                  uint      seed)
{
  Array weight = white_sparse(shape, 0.f, 1.f, density, seed);
  Array kernel = gabor({width, width}, kw, angle);

  return convolve2d_svd(weight, kernel, SVD_RANK);
}

} // namespace hmap
