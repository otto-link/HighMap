/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/convolve.hpp"
#include "highmap/kernels.hpp"

namespace hmap
{

void downscale_transform(Array                        &array,
                         float                         kc,
                         std::function<void(Array &x)> unary_op,
                         bool                          apply_prefiltering)
{
  int       nc_x = std::min(array.shape.x, std::max(4, (int)(2.f * kc)));
  int       nc_y = std::min(array.shape.y, std::max(4, (int)(2.f * kc)));
  Vec2<int> shape_coarse(nc_x, nc_y);

  Array array_filtered;

  if (apply_prefiltering)
  {
    // prepare sinc kernel
    float wavelengths_per_kernel = 4;
    int   nk = std::max(
        1,
        static_cast<int>(wavelengths_per_kernel * array.shape.x / kc));
    Vec2<int> kernel_shape(nk, nk);

    Array kernel = sinc_separable(kernel_shape, wavelengths_per_kernel);
    kernel *= blackman(kernel_shape);
    kernel /= kernel.sum();

    int rank = 1;
    array_filtered = convolve2d_svd(array, kernel, rank);
  }
  else
  {
    array_filtered = array.resample_to_shape(shape_coarse)
                         .resample_to_shape_bicubic(array.shape);
  }

  Array array_coarse = array_filtered.resample_to_shape(shape_coarse);

  // apply transformation
  unary_op(array_coarse);

  // upsample
  array_coarse = array_coarse.resample_to_shape_bicubic(array.shape);

  // replace input low-frequency content by the one transformed
  array = array_coarse + (array - array_filtered);
}

void downscale_transform_multi(
    Array                                                 &array,
    std::vector<float>                                     kc_list,
    std::function<void(Array &x, const int current_index)> unary_op,
    bool                                                   apply_prefiltering)
{
  int current_index = 0;

  for (auto kc : kc_list)
  {
    LOG_DEBUG("kc: %f", kc);

    auto unary_op_single = [current_index, &unary_op](Array &x)
    { unary_op(x, current_index); };
    current_index++;

    downscale_transform(array, kc, unary_op_single, apply_prefiltering);
  }
}

} // namespace hmap
