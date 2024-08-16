/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file range.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for range control functions and utilities.
 *
 * This header file contains function declarations and utilities for controlling
 * and manipulating the range of data. The provided functions are typically used
 * to constrain, normalize, or adjust data values to fall within specified
 * ranges.
 *
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Set to zero any value lower than `vmin`.
 *
 * @param array Input array.
 * @param vmin Lower bound.
 */
void chop(Array &array, float vmin);

/**
 * @brief Set to zero any value lower than `vmax` and apply a linear decrease
 * slope between `vmax / 2` and `vmax`.
 *
 * @param array Input array.
 * @param vmax Upper bound.
 */
void chop_max_smooth(Array &array, float vmax);

/**
 * @brief Clamp array elements to a target range.
 *
 * @todo Smooth clamping.
 *
 * @param array Input array.
 * @param vmin Lower bound of the clamping range.
 * @param vmax Upper bound of the clamping range.
 *
 * **Example**
 * @include ex_clamp.cpp
 *
 * **Result**
 * @image html ex_clamp.png
 *
 * @see {@link remap}, {@link clamp_min}, {@link clamp_max}
 */
void clamp(Array &array, float vmin = 0, float vmax = 1);

/**
 * @brief Clamp array values lower than a given bound.
 *
 * @param array Input array.
 * @param vmin Lower bound.
 *
 * @see {@link clamp}, {@link clamp_max}
 */
void clamp_min(Array &array, float vmin);
void clamp_min(Array &array, const Array &vmin); ///< @overload

/**
 * @brief Clamp array values lower than a given bound with a smooth transition.
 *
 * @param array Input array.
 * @param vmin Lower bound.
 * @param k Smoothing parameter in [0, 1].
 */
void clamp_min_smooth(Array &array, float vmin, float k = 0.2f);

void clamp_min_smooth(Array       &array,
                      const Array &vmin,
                      float        k = 0.2f); ///< @overload

/**
 * @brief Clamp value lower than a given bound with a smooth transition.
 *
 * @param x Input value.
 * @param vmin Lower bound.
 * @param k Smoothing parameter in [0, 1].
 */
float clamp_min_smooth(float x, float vmin, float k = 0.2f);

/**
 * @brief Clamp array values larger than a given bound.
 *
 * @param array Input array.
 * @param vmax Upper bound.
 *
 * @see {@link clamp}, {@link clamp_min}
 */
void clamp_max(Array &array, float vmax);
void clamp_max(Array &array, const Array &vmax); ///< @overload

/**
 * @brief Clamp array values larger than a given bound with a smooth transition.
 *
 * @param array Input array.
 * @param vmax Upper bound.
 * @param k Smoothing parameter in [0, 1].
 */
void clamp_max_smooth(Array &array, float vmax, float k = 0.2f);

void clamp_max_smooth(Array       &array,
                      const Array &vmax,
                      float        k = 0.2f); ///< @overload

/**
 * @brief Clamp array values within a given interval with a smooth transition.
 *
 * @param array Input array.
 * @param vmin Lower bound.
 * @param vmax Upper bound.
 * @param k Smoothing parameter in [0, 1].
 */
void clamp_smooth(Array &array, float vmin, float vmax, float k = 0.2f);

/**
 * @brief Return the element-wise maximum of two arrays.
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @return Array Element-wise maximum array.
 */
Array maximum(const Array &array1, const Array &array2);
Array maximum(const Array &array1, const float value); ///< @overload

/**
 * @brief Return the polynomial cubic smooth element-wise maximum of two arrays.
 *
 * Smoothly blend the two arrays to get rid of the discontinuity of the
 * {@link minimum} function (see <a
 * href=https://iquilezles.org/articles/smin/>Inigo Quilez's articles</a>).
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @param k Smoothing parameter in [0, 1].
 * @return Array Element-wise smooth minimum between the two input arrays.
 *
 * @see {@link minimum_smooth}, {@link minimum}, {@link maximum}
 */
Array maximum_smooth(const Array &array1, const Array &array2, float k = 0.2);
float maximum_smooth(const float a,
                     const float b,
                     float       k = 0.2); ///< @overload

/**
 * @brief Return the element-wise minimum of two arrays.
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @return Array Element-wise minimum array.
 */
Array minimum(const Array &array1, const Array &array2);

Array minimum(const Array &array1, const float value); ///< @overload

/**
 * @brief Return the polynomial cubic smooth element-wise minimum of two arrays.
 *
 * Smoothly blend the two arrays to get rid of the discontinuity of the
 * {@link minimum} function (see <a
 * href=https://iquilezles.org/articles/smin/>Inigo Quilez's articles</a>).
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @param k Smoothing parameter in [0, 1].
 * @return Array Element-wise smooth minimum between the two input arrays.
 *
 * @see {@link maximum_smooth}, {@link minimum}, {@link maximum}
 */
Array minimum_smooth(const Array &array1, const Array &array2, float k = 0.2);
float minimum_smooth(const float a, const float b, float k); ///< @overload

/**
 * @brief Remap array elements from a starting range to a target range.
 *
 * By default the starting range is taken to be [min(), max()] of the input
 * array.
 *
 * @param array Input array.
 * @param vmin The lower bound of the range to remap to.
 * @param vmax The lower bound of the range to remap to.
 * @param from_min The lower bound of the range to remap from.
 * @param from_max The upper bound of the range to remap from.
 *
 *  * **Example**
 * @include ex_remap.cpp
 *
 * @see {@link clamp}
 */
void remap(Array &array,
           float  vmin,
           float  vmax,
           float  from_min,
           float  from_max);

void remap(Array &array, float vmin = 0, float vmax = 1); ///< @overload

/**
 * @brief Remap array elements from a starting range to a target range.
 *
 * @param array Input array.
 * @param scaling Amplitude scaling.
 * @param vref Reference 'zero' value.
 */
void rescale(Array &array, float scaling, float vref = 0.f);

} // namespace hmap