/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

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

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Mask adjustement using a 'scanning' method.
 *
 * See https://www.shadertoy.com/view/stjSRR
 *
 * @param array Input array.
 * @param contrast Contrast.
 * @param brightness Brightness.
 * @return Ouput array.
 *
 * **Example**
 * @include ex_scan_mask.cpp
 *
 * **Result**
 * @image html ex_scan_mask.png
 */
Array scan_mask(const Array &array,
                float        contrast = 0.5f,
                float        brightness = 0.5f);

/**
 * @brief Return angle selection for a given angle and a tolerance half-width
 * on this value.
 * @param array Input array.
 * @param angle Selected angle (degree).
 * @param sigma Selected angle tolerance (degree).
 * @param ir Prefilter radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_select_angle.cpp
 *
 * **Result**
 * @image html ex_select_angle.png
 */
Array select_angle(const Array &array, float angle, float sigma, int ir = 0);

/**
 * @brief Return blob detection using the Laplacian of Gaussian (LoG) approach.
 *
 * @param array Input array.
 * @param ir Kernel radius.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select_blob.cpp
 *
 * **Result**
 * @image html ex_select_blob0.png
 * @image html ex_select_blob1.png
 */
Array select_blob_log(const Array &array, int ir);

/**
 * @brief Return holes or bumps detection based on the mean curvature of the
 * heightmap.
 *
 * @param array Input array.
 * @param ir Kernel radius.
 * @param concave Select 'holes' if set to true, and select 'bumps' if set to
 * false.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select_cavities.cpp
 *
 * **Result**
 * @image html ex_select_cavities.png
 */
Array select_cavities(const Array &array, int ir, bool concave = true);

/**
 * @brief
 *
 * @param array
 * @return Array
 *
 * **Example**
 * @include ex_select_elevation_slope.cpp
 *
 * **Result**
 * @image html ex_select_elevation_slope0.png
 * @image html ex_select_elevation_slope1.png
 */
Array select_elevation_slope(const Array &array, float gradient_scale);
Array select_elevation_slope(const Array &array,
                             float        gradient_scale,
                             float        vmax); ///< @overload

/**
 * @brief Return an array with elements equal to 1 where input elements are
 * equal to `value`.
 *
 * @param array Input array.
 * @param value Criteria value.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select.cpp
 *
 * **Result**
 * @image html ex_select0.png
 * @image html ex_select1.png
 * @image html ex_select2.png
 * @image html ex_select3.png
 * @image html ex_select4.png
 */
Array select_eq(const Array &array, float value);

/**
 * @brief Return an array weighted by the gap between the gradient angle and a
 * given angle.
 *
 * @param array Input array.
 * @param angle Reference angle (degree).
 * @return Array Output array.
 */
Array select_gradient_angle(const Array &array, float angle);

/**
 * @brief Return an array filled with 1 where the gradient is larger than a
 * given value and 0 elsewhere.
 *
 * @param array Input array.
 * @param talus_center Reference talus.
 * @return Array Output array.
 */
Array select_gradient_binary(const Array &array, float talus_center);

/**
 * @brief Return an array weighted (exponantial decay) by the gradient norm of
 * the input array.
 *
 * @param array Input array.
 * @param talus_center Reference talus.
 * @param talus_sigma Talus half-width.
 * @return Array Output array.
 */
Array select_gradient_exp(const Array &array,
                          float        talus_center,
                          float        talus_sigma);

/**
 * @brief Return an array weighted (square inverse) by the gradient norm of the
 * input array.
 *
 * @param array Input array.
 * @param talus_center Reference talus.
 * @param talus_sigma Talus half-width.
 * @return Array Output array.
 */
Array select_gradient_inv(const Array &array,
                          float        talus_center,
                          float        talus_sigma);

/**
 * @brief Return an array with elements equal to 1 where input elements are
 * larger than `value`.
 *
 * @param array Input array.
 * @param value Criteria value.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select.cpp
 *
 * **Result**
 * @image html ex_select0.png
 * @image html ex_select1.png
 * @image html ex_select2.png
 * @image html ex_select3.png
 * @image html ex_select4.png
 */
Array select_gt(const Array &array, float value);

/**
 * @brief Return an array with elements equal to 1 where input elements are
 * within the bounds provided.
 *
 * @param array Input array.
 * @param value1 Lower bound.
 * @param value2 Upper bound.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select.cpp
 *
 * **Result**
 * @image html ex_select0.png
 * @image html ex_select1.png
 * @image html ex_select2.png
 * @image html ex_select3.png
 * @image html ex_select4.png
 */
Array select_interval(const Array &array, float value1, float value2);

/**
 * @brief Return an array with positive values if the slope is pointing to the
 * center (slope is inward), and negative values otherwise (slope is outward).
 * @param array Input array.
 * @param center Reference center.
 * @param bbox Domain bounding box.
 * @return Output array.
 *
 * **Example**
 * @include ex_select_inward_outward_slope.cpp
 *
 * **Result**
 * @image html ex_select_inward_outward_slope.png
 */
Array select_inward_outward_slope(const Array &array,
                                  Vec2<float>  center = {0.5f, 0.5f},
                                  Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return an array with elements equal to 1 where input elements are
 * smaller than `value`.
 *
 * @param array Input array.
 * @param value Criteria value.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select.cpp
 *
 * **Result**
 * @image html ex_select0.png
 * @image html ex_select1.png
 * @image html ex_select2.png
 * @image html ex_select3.png
 * @image html ex_select4.png
 */
Array select_lt(const Array &array, float value);

/**
 * @brief Return an array filled with non-zero values where the input is in the
 * interval [value - sigma, value + sigma]. Output array values have a cubic
 * pulse distribution within this interval.
 *
 * @param array Input array.
 * @param value Central value.
 * @param sigma Pulse width.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select_pulse.cpp
 *
 * **Result**
 * @image html ex_select_pulse.png
 */
Array select_pulse(const Array &array, float value, float sigma);

/**
 * @brief Return an array filled with a criterion based on the occurence of a
 * river bed.
 *
 * @param array Input array.
 * @param talus_ref Reference talus used to localy define the flow-partition.
 * @param clipping_ratio Flow accumulation clipping ratio.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select_rivers.cpp
 *
 * **Result**
 * @image html ex_select_rivers.png
 */
Array select_rivers(const Array &array, float talus_ref, float clipping_ratio);

/**
 * @brief Return an array filled with 1 at the blending transition between two
 * arrays, and 0 elsewhere.
 *
 * @param array1 Input array 1.
 * @param array2 Input array 2.
 * @param array_blend Blending of array 1 and 2 to analyze.
 * @return Array Resulting array
 *
 * **Example**
 * @include ex_select_transitions.cpp
 *
 * **Result**
 * @image html ex_select_transitions0.png
 * @image html ex_select_transitions1.png
 * @image html ex_select_transitions2.png
 */
Array select_transitions(const Array &array1,
                         const Array &array2,
                         const Array &array_blend);

} // namespace hmap
