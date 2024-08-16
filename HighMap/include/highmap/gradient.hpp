/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file gradient.hpp
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
 * @brief Return the gradient of a vector.
 *
 * @param v Input vector.
 * @return std::vector<float> Vector gradient.
 */
std::vector<float> gradient1d(const std::vector<float> &v);

/**
 * @brief Return the polar angle of the gradient of an array.
 *
 * @param array Input array.
 * @param downward If set set true, return the polar angle of the downward
 * slope.
 * @return Array Gradient angle, in radians, in [-\pi, \pi].
 */
Array gradient_angle(const Array &array, bool downward = false);

/**
 * @brief Return the gradient norm of an array.
 *
 * @param array Input array.
 * @param p_dx Reference to the x-gradient (output).
 * @param p_dy Reference to the y-gradient (output).
 * @return Array Gradient norm.
 *
 * **Example**
 * @include ex_gradient_norm.cpp
 *
 * **Result**
 * @image html ex_gradient_norm.png
 */
Array gradient_norm(const Array &array,
                    Array       *p_dx = nullptr,
                    Array       *p_dy = nullptr);

/**
 * @brief Return the gradient norm of an array using Prewitt filter.
 *
 * @param array Input array.
 * @param p_dx Reference to the x-gradient (output).
 * @param p_dy Reference to the y-gradient (output).
 * @return Array Gradient norm.
 *
 * **Example**
 * @include ex_gradient_norm.cpp
 *
 * **Result**
 * @image html ex_gradient_norm.png
 */
Array gradient_norm_prewitt(const Array &array,
                            Array       *p_dx = nullptr,
                            Array       *p_dy = nullptr);

Array gradient_norm_scharr(const Array &array,
                           Array       *p_dx = nullptr,
                           Array       *p_dy = nullptr);

Array gradient_norm_sobel(const Array &array,
                          Array       *p_dx = nullptr,
                          Array       *p_dy = nullptr);

/**
 * @brief Return the gradient talus slope of an array.
 *
 * Talus slope is locally define as the largest elevation difference between a
 * cell and its first neighbors.
 *
 * @see Thermal erosion: {@link thermal}.
 *
 * @param array Input array.
 * @return Array Gradient.
 */
Array gradient_talus(const Array &array);

void gradient_talus(const Array &array, Array &talus); ///< @overload

/**
 * @brief Return the gradient in the 'x' (or 'i' index) of an array.
 *
 * @param array Input array.
 * @return Array Gradient.
 */
Array gradient_x(const Array &array);

void gradient_x(const Array &array, Array &dx); ///< @overload

/**
 * @brief Return the gradient in the 'y' (or 'j' index) of an array.
 *
 * @param array Input array.
 * @return Array Gradient.
 */
Array gradient_y(const Array &array);

void gradient_y(const Array &array, Array &dy); ///< @overload

/**
 * @brief Return the Laplacian of the input array.
 *
 * @param array Input array.
 * @return Array Laplacian array.
 */
Array laplacian(const Array &array);

} // namespace hmap