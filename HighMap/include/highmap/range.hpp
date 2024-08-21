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
 * This function processes the input array and sets any value that is less than
 * the specified lower bound `vmin` to zero. This operation effectively "chops
 * off" values that fall below the threshold, which can be useful for filtering
 * out unwanted low values in data processing tasks.
 *
 * @param array Input array to be processed. Values lower than `vmin` will be
 * set to zero.
 * @param vmin Lower bound threshold. Values below this threshold will be set to
 * zero.
 *
 * **Example**
 * @include ex_chop.cpp
 *
 * **Result**
 * @image html ex_chop.png
 */
void chop(Array &array, float vmin);

/**
 * @brief Set to zero any value lower than `vmax` and apply a linear decrease
 * slope between `vmax / 2` and `vmax`.
 *
 * This function processes the input array and sets any value below the upper
 * bound `vmax` to zero. Additionally, it applies a linear decrease slope to
 * values that fall between `vmax / 2` and `vmax`. This creates a smooth
 * transition where values gradually approach zero as they get closer to `vmax`,
 * providing a more nuanced adjustment compared to a sharp cutoff.
 *
 * @param array Input array to be processed. Values below `vmax` will be
 * adjusted, with a linear decrease applied between `vmax / 2` and `vmax`.
 * @param vmax Upper bound threshold. Values below this threshold will be
 * zeroed, with linear decrease applied in the specified range.
 *
 * **Example**
 * @include ex_chop_max_smooth.cpp
 *
 * **Result**
 * @image html ex_chop_max_smooth.png
 */
void chop_max_smooth(Array &array, float vmax);

/**
 * @brief Clamp array elements to a target range.
 *
 * This function restricts the values in the input array to be within a
 * specified range. Values below `vmin` are set to `vmin`, and values above
 * `vmax` are set to `vmax`. This operation ensures that all elements of the
 * array remain within the defined bounds.
 *
 * @param array Input array to be clamped.
 * @param vmin Lower bound of the clamping range (default is 0).
 * @param vmax Upper bound of the clamping range (default is 1).
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
 * This function clamps all values in the input array that are below the
 * specified lower bound `vmin` to the value of `vmin`.
 *
 * @param array Input array to be clamped.
 * @param vmin Lower bound for clamping.
 *
 * @see {@link clamp}, {@link clamp_max}
 */
void clamp_min(Array &array, float vmin);
void clamp_min(Array &array, const Array &vmin); ///< @overload

/**
 * @brief Clamp array values lower than a given bound with a smooth transition.
 *
 * This function smoothly clamps values in the input array that are below the
 * specified lower bound `vmin`. The smoothing parameter `k` controls the degree
 * of smoothness applied during the clamping process.
 *
 * @param array Input array to be clamped.
 * @param vmin Lower bound for clamping.
 * @param k Smoothing parameter in the range [0, 1] (default is 0.2f).
 */
void clamp_min_smooth(Array &array, float vmin, float k = 0.2f);

void clamp_min_smooth(Array       &array,
                      const Array &vmin,
                      float        k = 0.2f); ///< @overload

/**
 * @brief Clamp a single value lower than a given bound with a smooth
 * transition.
 *
 * This function smoothly clamps a single value that is below the specified
 * lower bound `vmin`. The smoothing parameter `k` controls the degree of
 * smoothness applied during the clamping process.
 *
 * @param x Input value to be clamped.
 * @param vmin Lower bound for clamping.
 * @param k Smoothing parameter in the range [0, 1] (default is 0.2f).
 * @return float The smoothly clamped value.
 */
float clamp_min_smooth(float x, float vmin, float k = 0.2f);

/**
 * @brief Clamp array values larger than a given bound.
 *
 * This function clamps all values in the input array that are above the
 * specified upper bound `vmax` to the value of `vmax`.
 *
 * @param array Input array to be clamped.
 * @param vmax Upper bound for clamping.
 *
 * @see {@link clamp}, {@link clamp_min}
 */
void clamp_max(Array &array, float vmax);
void clamp_max(Array &array, const Array &vmax); ///< @overload

/**
 * @brief Clamp array values larger than a given bound with a smooth transition.
 *
 * This function smoothly clamps values in the input array that are above the
 * specified upper bound `vmax`. The smoothing parameter `k` controls the degree
 * of smoothness applied during the clamping process.
 *
 * @param array Input array to be clamped.
 * @param vmax Upper bound for clamping.
 * @param k Smoothing parameter in the range [0, 1] (default is 0.2f).
 */
void clamp_max_smooth(Array &array, float vmax, float k = 0.2f);

void clamp_max_smooth(Array       &array,
                      const Array &vmax,
                      float        k = 0.2f); ///< @overload

/**
 * @brief Clamp array values within a given interval with a smooth transition.
 *
 * This function smoothly clamps values in the input array to be within a
 * specified range. Values below `vmin` are smoothly transitioned to `vmin`, and
 * values above `vmax` are smoothly transitioned to `vmax`. The smoothing
 * parameter `k` controls the degree of smoothness applied during the clamping
 * process.
 *
 * @param array Input array to be clamped.
 * @param vmin Lower bound of the clamping range.
 * @param vmax Upper bound of the clamping range.
 * @param k Smoothing parameter in the range [0, 1] (default is 0.2f).
 */
void clamp_smooth(Array &array, float vmin, float vmax, float k = 0.2f);

/**
 * @brief Return the element-wise maximum of two arrays.
 *
 * This function computes the element-wise maximum between two input arrays.
 * Each element in the resulting array is the maximum of the corresponding
 * elements from `array1` and `array2`.
 *
 * @param array1 First input array.
 * @param array2 Second input array.
 * @return Array The element-wise maximum between `array1` and `array2`.
 */
Array maximum(const Array &array1, const Array &array2);

/**
 * @brief Return the element-wise maximum of an array and a scalar value.
 *
 * This overloaded function computes the element-wise maximum between an input
 * array and a scalar value. Each element in the resulting array is the maximum
 * of the corresponding element from the input array and the scalar value.
 *
 * @param array1 Input array.
 * @param value Scalar value to compare with each element of the array.
 * @return Array The element-wise maximum between `array1` and `value`.
 */
Array maximum(const Array &array1, const float value); ///< @overload

/**
 * @brief Return the polynomial cubic smooth element-wise maximum of two arrays.
 *
 * This function computes the element-wise maximum of two input arrays with a
 * smooth transition to avoid discontinuities that are present in the standard
 * `maximum` function. The smoothness is controlled by the parameter `k`, which
 * determines how smoothly the transition between the two arrays occurs.
 *
 * The smooth transition is achieved using a polynomial cubic function, which
 * provides a smooth blend between the two arrays. This approach helps in
 * avoiding abrupt changes and can be useful in applications requiring smooth
 * transitions.
 *
 * @param array1 First array to be compared.
 * @param array2 Second array to be compared.
 * @param k Smoothing parameter in the range [0, 1]. Higher values result in
 * smoother transitions between the arrays (default is 0.2).
 * @return Array Element-wise smooth maximum between the two input arrays.
 *
 * **Example**
 * @include ex_maximum_smooth.cpp
 *
 * **Result**
 * @image html ex_maximum_smooth.png
 *
 * @see {@link minimum_smooth}, {@link minimum}, {@link maximum}
 */
Array maximum_smooth(const Array &array1, const Array &array2, float k = 0.2);

/**
 * @brief Return the polynomial cubic smooth maximum of two scalar values.
 *
 * This function computes the smooth maximum of two scalar values with a
 * polynomial cubic smooth transition. The parameter `k` controls the degree of
 * smoothness applied to the maximum calculation, ensuring a continuous and
 * smooth blend between the two values.
 *
 * The smooth transition helps in avoiding abrupt changes, making this function
 * suitable for applications that require smooth variations.
 *
 * @param a First scalar value.
 * @param b Second scalar value.
 * @param k Smoothing parameter in the range [0, 1]. Higher values result in
 * smoother transitions between the values (default is 0.2).
 * @return float Smooth maximum value between the two input values.
 *
 * **Example**
 * @include ex_maximum_smooth_scalar.cpp
 *
 * **Result**
 * @image html ex_maximum_smooth_scalar.png
 *
 * @see {@link minimum_smooth}, {@link minimum}, {@link maximum}
 */
float maximum_smooth(const float a,
                     const float b,
                     float       k = 0.2); ///< @overload

/**
 * @brief Return the element-wise minimum of two arrays.
 *
 * This function computes the element-wise minimum between two input arrays.
 * Each element in the resulting array is the minimum of the corresponding
 * elements from `array1` and `array2`.
 *
 * @param array1 First input array.
 * @param array2 Second input array.
 * @return Array The element-wise minimum between `array1` and `array2`.
 */
Array minimum(const Array &array1, const Array &array2);

/**
 * @brief Return the element-wise minimum of an array and a scalar value.
 *
 * This overloaded function computes the element-wise minimum between an input
 * array and a scalar value. Each element in the resulting array is the minimum
 * of the corresponding element from the input array and the scalar value.
 *
 * @param array1 Input array.
 * @param value Scalar value to compare with each element of the array.
 * @return Array The element-wise minimum between `array1` and `value`.
 */
Array minimum(const Array &array1, const float value); ///< @overload

/**
 * @brief Return the polynomial cubic smooth element-wise minimum of two arrays.
 *
 * This function computes a smooth element-wise minimum between two input arrays
 * using a polynomial cubic smoothing function. The smoothing parameter `k`
 * controls the blending between the arrays, reducing the discontinuity found in
 * a standard minimum operation. For details on the smoothing technique, refer
 * to [Inigo Quilez's articles](https://iquilezles.org/articles/smin/).
 *
 * @param array1 First input array.
 * @param array2 Second input array.
 * @param k Smoothing parameter in the range [0, 1] that determines the degree
 * of blending between the two arrays (default is 0.2).
 * @return Array The element-wise smooth minimum between `array1` and `array2`.
 *
 * @see {@link maximum_smooth}, {@link minimum}, {@link maximum}
 */
Array minimum_smooth(const Array &array1, const Array &array2, float k = 0.2);

/**
 * @brief Return the polynomial cubic smooth minimum of two scalar values.
 *
 * This overloaded function computes a smooth minimum between two scalar values
 * using a polynomial cubic smoothing function. The smoothing parameter `k`
 * controls the blending between the values, reducing discontinuity compared to
 * a standard minimum operation.
 *
 * @param a First scalar value.
 * @param b Second scalar value.
 * @param k Smoothing parameter in the range [0, 1] that determines the degree
 * of blending between the two values (default is 0.2).
 * @return float The smooth minimum between `a` and `b`.
 */
float minimum_smooth(const float a, const float b, float k); ///< @overload

/**
 * @brief Remap array elements from a starting range to a target range.
 *
 * This function maps the values in the input array from one specified range
 * to another. By default, the function uses the minimum and maximum values
 * of the input array as the starting range. The target range is specified
 * by `vmin` and `vmax`.
 *
 * @param array Input array to be remapped.
 * @param vmin The lower bound of the target range to remap to.
 * @param vmax The upper bound of the target range to remap to.
 * @param from_min The lower bound of the starting range to remap from.
 * @param from_max The upper bound of the starting range to remap from.
 *
 * **Example**
 * @include ex_remap.cpp
 *
 * @see {@link clamp}
 */
void remap(Array &array,
           float  vmin,
           float  vmax,
           float  from_min,
           float  from_max);

/**
 * @brief Remap array elements from a starting range to a target range (default
 * range).
 *
 * This overloaded function remaps the values in the input array from the
 * default starting range (the minimum and maximum values of the array)
 * to a target range specified by `vmin` and `vmax`.
 *
 * @param array Input array to be remapped.
 * @param vmin The lower bound of the target range to remap to (default is 0).
 * @param vmax The upper bound of the target range to remap to (default is 1).
 */
void remap(Array &array, float vmin = 0, float vmax = 1); ///< @overload

/**
 * @brief Remap array elements from a starting range to a target range.
 *
 * This function adjusts the values in the input array by scaling them and
 * shifting them to fit within a new range. The scaling factor determines how
 * much the values are stretched or compressed, and the reference value shifts
 * the values to align with the target range.
 *
 * @param array Input array to be rescaled.
 * @param scaling Amplitude scaling factor to adjust the range of array
 * elements.
 * @param vref Reference 'zero' value used to shift the values (default is 0.f).
 */
void rescale(Array &array, float scaling, float vref = 0.f);

} // namespace hmap