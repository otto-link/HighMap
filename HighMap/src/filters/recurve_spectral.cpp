/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/fft.hpp"
#include "highmap/interpolate1d.hpp"

namespace hmap
{

void recurve_spectral(Array &array, const std::vector<float> &weights)
{
  if (weights.size() < 2) return;

  // work on a square array because of the fft filter
  int n = std::max(array.shape.x, array.shape.y);

  if (array.shape.x != array.shape.y)
  {
    Array array_wrk = array.resample_to_shape(Vec2<int>(n, n));
    Array array_filtered = fft_filter(array_wrk, weights);
    array = array_filtered.resample_to_shape(array.shape);
  }
  else
    array = fft_filter(array, weights);
}

} // namespace hmap
