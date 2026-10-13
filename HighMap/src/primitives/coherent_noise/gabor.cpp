/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdint>

#include "highmap/array.hpp"
#include "highmap/internal/validation.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math/convolve.hpp"
#include "highmap/primitives/coherent_noise.hpp"
#include "highmap/primitives/random.hpp"

#define SVD_RANK 2

namespace hmap
{

Array gabor_noise(glm::ivec2    shape,
                  float         kw,
                  float         angle,
                  int           width,
                  float         density,
                  std::uint32_t seed)
{
  if (!validate_shape(shape)) return Array();

  Array weight = white_sparse(shape, 0.f, 1.f, density, seed);
  Array kernel = gabor({width, width}, kw, angle);

  return convolve2d_svd(weight, kernel, SVD_RANK);
}

} // namespace hmap
