/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file gradient.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Provides functions for calculating gradients and related operations on
 * arrays.
 * @version 0.1
 * @date 2023-04-30
 *
 * This header file defines functions to compute various gradient operations
 * including gradient norms, gradient angles, and Laplacians for 2D arrays.
 * It supports different gradient filters such as Prewitt, Sobel, and Scharr.
 *
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Compute the gradient of a 1D vector.
 *
 * This function calculates the gradient of a 1D vector by computing the
 * difference between successive elements.
 *
 * @param v Input vector of floats.
 * @return std::vector<float> Vector of gradient values.
 */
std::vector<float> gradient1d(const std::vector<float> &v);

/**
 * @brief Compute the polar angle of the gradient of a 2D array.
 *
 * This function calculates the gradient angle of a 2D array, which represents
 * the direction of the steepest ascent. The angle is measured in radians and
 * falls within the range [-π, π]. The direction can be inverted if the
 * `downward` flag is set to true.
 *
 * @param array Input 2D array.
 * @param downward If true, invert the direction of the gradient.
 * @return Array Gradient angles in radians.
 */
Array gradient_angle(const Array &array, bool downward = false);

/**
 * @brief Compute the gradient norm of a 2D array.
 *
 * This function calculates the gradient norm (magnitude) of a 2D array.
 * Optionally, the x and y gradients can be computed and stored in `p_dx` and
 * `p_dy` respectively.
 *
 * @param array Input 2D array.
 * @param p_dx Pointer to an Array where the x-gradient will be stored
 * (optional).
 * @param p_dy Pointer to an Array where the y-gradient will be stored
 * (optional).
 * @return Array Gradient norm (magnitude).
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
 * @brief Compute the gradient norm of a 2D array using the Prewitt filter.
 *
 * This function calculates the gradient norm (magnitude) using the Prewitt
 * filter for gradient computation. Optionally, the x and y gradients can be
 * computed and stored in `p_dx` and `p_dy` respectively.
 *
 * @param array Input 2D array.
 * @param p_dx Pointer to an Array where the x-gradient will be stored
 * (optional).
 * @param p_dy Pointer to an Array where the y-gradient will be stored
 * (optional).
 * @return Array Gradient norm (magnitude).
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

/**
 * @brief Compute the gradient norm of a 2D array using the Scharr filter.
 *
 * This function calculates the gradient norm (magnitude) using the Scharr
 * filter for gradient computation. Optionally, the x and y gradients can be
 * computed and stored in `p_dx` and `p_dy` respectively.
 *
 * @param array Input 2D array.
 * @param p_dx Pointer to an Array where the x-gradient will be stored
 * (optional).
 * @param p_dy Pointer to an Array where the y-gradient will be stored
 * (optional).
 * @return Array Gradient norm (magnitude).
 */
Array gradient_norm_scharr(const Array &array,
                           Array       *p_dx = nullptr,
                           Array       *p_dy = nullptr);

/**
 * @brief Compute the gradient norm of a 2D array using the Sobel filter.
 *
 * This function calculates the gradient norm (magnitude) using the Sobel filter
 * for gradient computation. Optionally, the x and y gradients can be computed
 * and stored in `p_dx` and `p_dy` respectively.
 *
 * @param array Input 2D array.
 * @param p_dx Pointer to an Array where the x-gradient will be stored
 * (optional).
 * @param p_dy Pointer to an Array where the y-gradient will be stored
 * (optional).
 * @return Array Gradient norm (magnitude).
 */
Array gradient_norm_sobel(const Array &array,
                          Array       *p_dx = nullptr,
                          Array       *p_dy = nullptr);

/**
 * @brief Compute the gradient talus slope of a 2D array.
 *
 * The talus slope is defined as the largest elevation difference between a
 * cell and its immediate neighbors. This function computes the talus slope
 * for the given 2D array.
 *
 * @param array Input 2D array.
 * @return Array Gradient talus slope.
 *
 * @see Thermal erosion: {@link thermal}.
 */
Array gradient_talus(const Array &array);

/**
 * @brief Compute the gradient talus slope and store it in the provided array.
 *
 * This overload computes the gradient talus slope of a 2D array and stores
 * the result in the given `talus` array.
 *
 * @param array Input 2D array.
 * @param talus Output array where the gradient talus slope will be stored.
 */
void gradient_talus(const Array &array, Array &talus); ///< @overload

/**
 * @brief Compute the gradient in the x-direction of a 2D array.
 *
 * This function calculates the gradient in the x-direction (i.e., horizontal
 * gradient) for the given 2D array.
 *
 * @param array Input 2D array.
 * @return Array Gradient in the x-direction.
 */
Array gradient_x(const Array &array);

/**
 * @brief Compute the gradient in the x-direction of a 2D array and store it.
 *
 * This overload calculates the gradient in the x-direction and stores the
 * result in the provided `dx` array.
 *
 * @param array Input 2D array.
 * @param dx Output array where the gradient in the x-direction will be stored.
 */
void gradient_x(const Array &array, Array &dx); ///< @overload

/**
 * @brief Compute the gradient in the y-direction of a 2D array.
 *
 * This function calculates the gradient in the y-direction (i.e., vertical
 * gradient) for the given 2D array.
 *
 * @param array Input 2D array.
 * @return Array Gradient in the y-direction.
 */
Array gradient_y(const Array &array);

/**
 * @brief Compute the gradient in the y-direction of a 2D array and store it.
 *
 * This overload calculates the gradient in the y-direction and stores the
 * result in the provided `dy` array.
 *
 * @param array Input 2D array.
 * @param dy Output array where the gradient in the y-direction will be stored.
 */
void gradient_y(const Array &array, Array &dy); ///< @overload

/**
 * @brief Compute the Laplacian of a 2D array.
 *
 * The Laplacian is a measure of the second-order spatial derivative of the
 * input array. This function computes the Laplacian for the given 2D array.
 *
 * @param array Input 2D array.
 * @return Array Laplacian of the input array.
 */
Array laplacian(const Array &array);

} // namespace hmap
