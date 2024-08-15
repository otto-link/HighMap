/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file transform.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "macrologger.h"

namespace hmap
{

/**
 * @brief Flip the array vertically (left/right).
 *
 * @param array Input array.
 *
 * **Example**
 * @include flip_ud.cpp
 *
 * **Result**
 * @image html flip_ud.png
 */
void flip_lr(Array &array);

/**
 * @brief Flip the array horizontally (up/down).
 *
 * @param array Input array.
 *
 * **Example**
 * @include flip_ud.cpp
 *
 * **Result**
 * @image html flip_ud.png
 */
void flip_ud(Array &array);

/**
 * @brief Rotate the array by 90 degrees.
 * @param array Input array.
 *
 * **Example**
 * @include ex_rot90.cpp
 *
 * **Result**
 * @image html ex_rot90.png
 */
void rot90(Array &array);

/**
 * @brief Rotate the array.
 *
 * @param array Input array.
 * @param angle Rotation angle in degrees.
 * @param zero_padding Use zero-padding instead of symmetry to fill values (can
 * be useful when rotating array with zero values at the domain borders).
 *
 * **Example**
 * @include ex_rotate.cpp
 *
 * **Result**
 * @image html ex_rotate.png
 */
void rotate(Array &array, float angle, bool zero_padding = false);

/**
 * @brief Return the transposed array.
 * @param array Input array.
 * @return Transposed array.
 */
Array transpose(const Array &array);

/** 
 * @brief Translates a 2D array by a specified amount along the x and y axes.
 *
 * This function shifts the contents of the input array by `dx` and `dy` units
 * along the x and y axes, respectively. It supports both periodic boundary
 * conditions, where the array wraps around, and non-periodic conditions, where
 * the shifted areas are filled with zeros.
 *
 * @param array The input 2D array to be translated. This array remains
 * unmodified.
 * @param dx The translation distance along the x-axis. Positive values shift
 * the array to the right.
 * @param dy The translation distance along the y-axis. Positive values shift
 * the array downward.
 * @param periodic If set to `true`, the translation is periodic, meaning that
 * elements that move out of one side of the array reappear on the opposite
 * side. If `false`, the areas exposed by the translation are filled with zeros.
 * The default is `false`.
 * @param p_noise_x Optional pointer to a 2D array that contains x-direction
 * noise to be added to the translation. If provided, the noise values are added
 * to `dx` on a per-element basis.
 * @param p_noise_y Optional pointer to a 2D array that contains y-direction
 * noise to be added to the translation. If provided, the noise values are added
 * to `dy` on a per-element basis.
 * @param bbox Domain bounding box.
 *
 * @return A new 2D array that is the result of translating the input `array` by
 * the specified `dx` and `dy` values.
 *
 * **Example**
 * @include ex_translate.cpp
 *
 * **Result**
 * @image html ex_translate.png
 */
Array translate(const Array &array,
                float        dx,
                float        dy,
                bool         periodic = false,
                Array       *p_noise_x = nullptr,
                Array       *p_noise_y = nullptr,
                Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Apply a warping effect to the array.
 *
 * @param array Input array.
 * @param p_dx Reference to the x translation array.
 * @param p_dy Reference to the y translation array.
 *
 * **Example**
 * @include ex_warp.cpp
 *
 * **Result**
 * @image html ex_warp.png
 */
void warp(Array &array, Array *p_dx, Array *p_dy);

/**
 * @brief Apply a warping effect following the downward local gradient direction
 * (deflate / inflate effect).
 *
 * @param array Input array.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param amount Amount of displacement.
 * @param ir Pre-filtering radius.
 * @param reverse Reverse displacement direction.
 *
 * **Example**
 * @include ex_warp_directional.cpp
 *
 * **Result**
 * @image html ex_warp_directional.png
 */
void warp_directional(Array &array,
                      float  angle,
                      float  amount = 0.02f,
                      int    ir = 4,
                      bool   reverse = false);

void warp_directional(Array &array,
                      float  angle,
                      Array *p_mask,
                      float  amount = 0.02f,
                      int    ir = 4,
                      bool   reverse = false); ///< @overload

/**
 * @brief Apply a warping effect following the downward local gradient direction
 * (deflate / inflate effect).
 *
 * @param array Input array.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param amount Amount of displacement.
 * @param ir Pre-filtering radius.
 * @param reverse Reverse displacement direction.
 *
 * **Example**
 * @include ex_warp_downslope.cpp
 *
 * **Result**
 * @image html ex_warp_downslope.png
 */
void warp_downslope(Array &array,
                    float  amount = 0.02f,
                    int    ir = 4,
                    bool   reverse = false);

void warp_downslope(Array &array,
                    Array *p_mask,
                    float  amount = 0.02f,
                    int    ir = 4,
                    bool   reverse = false); ///< @overload

/**
 * @brief Applies a zoom effect to a 2D array with an adjustable center.
 *
 * This function scales the input 2D array by a specified zoom factor,
 * effectively resizing the array's contents. The zoom operation is centered
 * around a specified point within the array, allowing for flexible zooming
 * behavior. The function supports both periodic boundary conditions, where the
 * array wraps around, and non-periodic conditions, where areas outside the
 * original array bounds are filled with zeros.
 *
 * @param array The input 2D array to be zoomed. This array remains unmodified.
 * @param zoom_factor The factor by which to zoom the array. A value greater
 * than 1 enlarges the contents, while a value between 0 and 1 reduces them.
 * @param periodic If set to `true`, the zoom is periodic, meaning that elements
 * moving out of the array bounds due to the zoom reappear on the opposite side.
 * If `false`, areas outside the original array bounds are filled with zeros.
 * The default is `false`.
 * @param center The center of the zoom operation, specified as a `Vec2<float>`
 * with coordinates in the range [0, 1], where {0.5f, 0.5f} represents the
 * center of the array. The default center is {0.5f, 0.5f}.
 * @param p_noise_x Optional pointer to a 2D array that contains x-direction
 * noise to be added during the zoom operation. If provided, the noise values
 * are applied to the x-coordinates of the zoomed array on a per-element basis.
 * @param p_noise_y Optional pointer to a 2D array that contains y-direction
 * noise to be added during the zoom operation. If provided, the noise values
 * are applied to the y-coordinates of the zoomed array on a per-element basis.
 * @param bbox Domain bounding box.
 *
 * @return A new 2D array that is the result of applying the zoom effect to the
 * input `array` by the specified `zoom_factor` and centered at the specified
 * `center`.
 *
 * **Example**
 * @include ex_zoom.cpp
 *
 * **Result**
 * @image html ex_zoom.png
 */
Array zoom(const Array &array,
           float        zoom_factor,
           bool         periodic = false,
           Vec2<float>  center = {0.5f, 0.5f},
           Array       *p_noise_x = nullptr,
           Array       *p_noise_y = nullptr,
           Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap