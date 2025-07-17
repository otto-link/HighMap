/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

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
 * @brief Applies an upscaling amplification process to an array, followed by a
 * unary operation.
 *
 * This function progressively upscales the given array by powers of 2 (starting
 * from the initial shape), performs a user-defined unary operation on the
 * upscaled array at each level, and finally returns the array to its original
 * resolution using bilinear interpolation.
 *
 * @param array            A reference to the array that will be upscaled and
 *                         processed.
 * @param upscaling_levels The number of upscaling levels to apply. The function
 *                         will resample the array for each level.
 * @param persistence      A scaling factor applied at each level to adjust the
 *                         impact of the unary operation. Higher persistence
 *                         values will amplify the effects at each level.
 * @param unary_op         A user-defined unary operation to apply to the array
 *                         at each upscaling level. The operation takes a
 *                         reference to the array.
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