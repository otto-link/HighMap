/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/fft.hpp"

namespace hmap
{

void downscale_transform(Array                        &array,
                         float                         kc,
                         std::function<void(Array &x)> unary_op)
{
  // work on a square array because of the fft filter
  int n = std::max(array.shape.x, array.shape.y);

  Array array_wrk = array;

  if (array.shape.x != array.shape.y)
    array_wrk = array.resample_to_shape(Vec2<int>(n, n));

  // filter and downsample
  bool  smooth_cutoff = true;
  Array array_filtered = fft_filter(array_wrk, kc, smooth_cutoff);

  int   nc = std::min(n, std::max(4, (int)(2.f * kc)));
  Array array_coarse = array_filtered.resample_to_shape(Vec2<int>(nc, nc));

  // apply transformation
  unary_op(array_coarse);

  // upsample
  array_coarse = array_coarse.resample_to_shape_bicubic(Vec2<int>(n, n));

  // replace input low-frequency content by the one transformed
  array_wrk = array_coarse + (array_wrk - array_filtered);

  if (array.shape.x != array.shape.y)
    array = array_wrk.resample_to_shape(array.shape);
  else
    array = array_wrk;
}

} // namespace hmap
