/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file upscale_amplification.hpp
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
 * @param array A reference to the input 2D array to be transformed. The array
 * may be resized internally.
 * @param kc The cutoff wavenumber for isolating low-frequency components during
 * filtering.
 * @param unary_op A user-defined function that applies a transformation to the
 * downscaled array.
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
                         std::function<void(Array &x)> unary_op);

/**
 * @brief Applies a downscaling transformation to a 2D array using spatial-based
 * filtering (sinc kernel).
 *
 * This function performs a series of operations on a 2D array to isolate and
 * transform its low-frequency components
 *
 * @param array A reference to the input 2D array to be transformed.
 * @param kc The cutoff wavenumber for isolating low-frequency components during
 * filtering.
 * @param unary_op A user-defined function that applies a transformation to the
 * downscaled array.
 *
 * * **Example**
 * @include ex_downscale_transform_sinc.cpp
 *
 * **Result**
 * @image html ex_downscale_transform_sinc.png
 */
void downscale_transform_sinc(Array                        &array,
                              float                         kc,
                              std::function<void(Array &x)> unary_op);

/**
 * @brief Applies an upscaling amplification process to an array, followed by a
 * unary operation.
 *
 * This function progressively upscales the given array by powers of 2 (starting
 * from the initial shape), performs a user-defined unary operation on the
 * upscaled array at each level, and finally returns the array to its original
 * resolution using bilinear interpolation.
 *
 * @param array A reference to the array that will be upscaled and processed.
 * @param upscaling_levels The number of upscaling levels to apply. The function
 * will resample the array for each level.
 * @param persistence A scaling factor applied at each level to adjust the
 * impact of the unary operation. Higher persistence values will amplify the
 * effects at each level.
 * @param unary_op A user-defined unary operation to apply to the array at each
 * upscaling level. The operation takes a reference to the array.
 *
 * @note The function first applies bicubic resampling to upscale the array,
 * then applies the user-provided `unary_op` at each upscaling level. After all
 * levels are processed, the array is resampled back to its initial shape using
 * bilinear interpolation.
 */
void upscale_amplification(
    Array                                               &array,
    int                                                  upscaling_levels,
    float                                                persistence,
    std::function<void(Array &x, float current_scaling)> unary_op);

} // namespace hmap