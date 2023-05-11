/**
 * @file op.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace hmap
{

/**
 * @brief Apply the almost unit identity function.
 *
 * Function that maps the unit interval to itself with zero derivative at 0 and
 * "one" derivative at 1 (see <a
 * href=https://iquilezles.org/articles/functions/>Inigo Quilez's articles</a>).
 *
 * @param array Input array.
 */
void almost_unit_identity(Array &array);

/**
 * @brief Clamp array elements to a target range.
 *
 * @todo Smooth clamping.
 *
 * @param array Input array (elements expected to be in [0, 1]).
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
 * @see {@link clamp}, {@link clamp_upper_bound}
 */
void clamp_min(Array &array, float vmin);

/**
 * @brief Clamp array values larger than a given bound.
 *
 * @param array Input array.
 * @param vmin Upper bound.
 *
 * @see {@link clamp}, {@link clamp_lower_bound}
 */
void clamp_max(Array &array, float vmax);

/**
 * @brief Linear extrapolation of values at the borders (i = 0, j = 0, ...)
 * based on inner values.
 *
 * @param array Input array.
 *
 * @see {@link fill_borders}
 *
 */
void extrapolate_borders(Array &array);

/**
 * @brief Fill values at the borders (i = 0, j = 0, ...) based on 1st neighbor
 * values.
 *
 * @param array Input array.
 *
 * @see {@link extrapolate_borders}
 */
void fill_borders(Array &array);

/**
 * @brief Apply a gain correction of the array elements.
 *
 * Gain correction is based on a power law.
 *
 * @param array Input array.
 * @param gain Gain factor (> 0).
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * **Example**
 * @include ex_gain.cpp
 *
 * **Result**
 * @image html ex_gain.png
 */
void gain(Array &array, float gain);

/**
 * @brief Apply gamma correction to the input array.
 *
 * @param array Input array.
 * @param gamma Gamma factor (> 0).
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * **Example**
 * @include ex_gamma_correction.cpp
 *
 * **Result**
 * @image html ex_gamma_correction.png
 */
void gamma_correction(Array &array, float gamma);

/**
 * @brief Return the element-wise maximum of two arrays.
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @return Array Element-wise maximum array.
 */
Array maximum(Array &array1, Array &array2);

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
Array maximum_smooth(Array &array1, Array &array2, float k = 0.2);

/**
 * @brief Return the element-wise minimum of two arrays.
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @return Array Element-wise minimum array.
 */
Array minimum(Array &array1, Array &array2);

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
Array minimum_smooth(Array &array1, Array &array2, float k = 0.2);

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
           float  from_max);                               ///< @overload
void remap(Array &array, float vmin = 0, float vmax = 1); ///< @overload

/**
 * @brief Apply sharpening filter (based on Laplace operator).
 *
 * @param array Input array.
 * @param ratio Output ratio between sharpened (ratio = 1) and non-sharpened
 * array (ratio = 0).
 *
 * **Example**
 * @include ex_sharpen.cpp
 *
 * **Result**
 * @image html ex_sharpen.png
 */
void sharpen(Array &array, float ratio = 1.f);

} // namespace hmap