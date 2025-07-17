/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file downscaling.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <functional>

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Applies a downscaling transformation to a 2D array using Fourier-based
 * filtering.
 *
 * This function performs a series of operations on a 2D array to isolate and
 * transform its low-frequency components. It uses a Fourier transform for
 * filtering, downscales the array for efficient processing, applies a
 * user-defined unary operation, and then restores the transformed low-frequency
 * content to the original resolution.
 *
 * @param array    A reference to the input 2D array to be transformed. The
 *                 array may be resized internally.
 * @param kc       The cutoff wavenumber for isolating low-frequency components
 *                 during filtering.
 * @param unary_op A user-defined function that applies a transformation to the
 *                 downscaled array.
 *
 * @details
 * - The function ensures that the input array is square for Fourier operations
 * by resampling it if necessary.
 * - A smooth low-pass filter is applied in the frequency domain, and the
 * filtered array is downscaled to a coarse resolution.
 * - The user-defined transformation (`unary_op`) is applied to the downscaled
 * array.
 * - The transformed low-frequency content is upsampled and reintegrated into
 * the original array.
 * - If the input array was not square, the final result is resampled back to
 * its original shape.
 *
 * * **Example**
 * @include ex_downscale_transform.cpp
 *
 * **Result**
 * @image html ex_downscale_transform.png
 */
void downscale_transform(Array                        &array,
                         float                         kc,
                         std::function<void(Array &x)> unary_op,
                         bool apply_prefiltering = false);

void downscale_transform_multi(
    Array                                                 &array,
    std::vector<float>                                     kc_list,
    std::function<void(Array &x, const int current_index)> unary_op,
    bool apply_prefiltering = false);

} // namespace hmap