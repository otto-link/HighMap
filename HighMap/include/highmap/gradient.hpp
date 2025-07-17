/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file gradient.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Provides functions for calculating gradients and related operations on
 * arrays. This header file defines functions to compute various gradient
 * operations including gradient norms, gradient angles, and Laplacians for 2D
 * arrays. It supports different gradient filters such as Prewitt, Sobel, and
 * Scharr.
 *
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once

#include "highmap/array.hpp"
#include "highmap/tensor.hpp"

namespace hmap
{

/**
 * @brief Compute the gradient of a 1D vector.
 *
 * This function calculates the gradient of a 1D vector by computing the
 * difference between successive elements.
 *
 * @param  v Input vector of floats.
 * @return   std::vector<float> Vector of gradient values.
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
 * @param  array    Input 2D array.
 * @param  downward If true, invert the direction of the gradient.
 * @return          Array Gradient angles in radians.
 */
Array gradient_angle(const Array &array, bool downward = false);

/**
 * @brief Compute the gradient norm of a 2D array.
 *
 * This function calculates the gradient norm (magnitude) of a 2D array.
 * Optionally, the x and y gradients can be computed and stored in `p_dx` and
 * `p_dy` respectively.
 *
 * @param  array Input 2D array.
 * @param  p_dx  Pointer to an Array where the x-gradient will be stored
 *               (optional).
 * @param  p_dy  Pointer to an Array where the y-gradient will be stored
 *               (optional).
 * @return       Array Gradient norm (magnitude).
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
 * @param  array Input 2D array.
 * @param  p_dx  Pointer to an Array where the x-gradient will be stored
 *               (optional).
 * @param  p_dy  Pointer to an Array where the y-gradient will be stored
 *               (optional).
 * @return       Array Gradient norm (magnitude).
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
 * @param  array Input 2D array.
 * @param  p_dx  Pointer to an Array where the x-gradient will be stored
 *               (optional).
 * @param  p_dy  Pointer to an Array where the y-gradient will be stored
 *               (optional).
 * @return       Array Gradient norm (magnitude).
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
 * @param  array Input 2D array.
 * @param  p_dx  Pointer to an Array where the x-gradient will be stored
 *               (optional).
 * @param  p_dy  Pointer to an Array where the y-gradient will be stored
 *               (optional).
 * @return       Array Gradient norm (magnitude).
 */
Array gradient_norm_sobel(const Array &array,
                          Array       *p_dx = nullptr,
                          Array       *p_dy = nullptr);

/**
 * @brief Compute the gradient talus slope of a 2D array.
 *
 * The talus slope is defined as the largest elevation difference between a cell
 * and its immediate neighbors. This function computes the talus slope for the
 * given 2D array.
 *
 * @param  array Input 2D array.
 * @return       Array Gradient talus slope.
 *
 * @see          Thermal erosion: {@link thermal}.
 */
Array gradient_talus(const Array &array);

/**
 * @brief Compute the gradient talus slope and store it in the provided array.
 *
 * This overload computes the gradient talus slope of a 2D array and stores the
 * result in the given `talus` array.
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
 * @param  array Input 2D array.
 * @return       Array Gradient in the x-direction.
 */
Array gradient_x(const Array &array);

/**
 * @brief Compute the gradient in the x-direction of a 2D array and store it.
 *
 * This overload calculates the gradient in the x-direction and stores the
 * result in the provided `dx` array.
 *
 * @param array Input 2D array.
 * @param dx    Output array where the gradient in the x-direction will be
 *              stored.
 */
void gradient_x(const Array &array, Array &dx); ///< @overload

/**
 * @brief Compute the gradient in the y-direction of a 2D array.
 *
 * This function calculates the gradient in the y-direction (i.e., vertical
 * gradient) for the given 2D array.
 *
 * @param  array Input 2D array.
 * @return       Array Gradient in the y-direction.
 */
Array gradient_y(const Array &array);

/**
 * @brief Compute the gradient in the y-direction of a 2D array and store it.
 *
 * This overload calculates the gradient in the y-direction and stores the
 * result in the provided `dy` array.
 *
 * @param array Input 2D array.
 * @param dy    Output array where the gradient in the y-direction will be
 *              stored.
 */
void gradient_y(const Array &array, Array &dy); ///< @overload

/**
 * @brief Compute the Laplacian of a 2D array.
 *
 * The Laplacian is a measure of the second-order spatial derivative of the
 * input array. This function computes the Laplacian for the given 2D array.
 *
 * @param  array Input 2D array.
 * @return       Array Laplacian of the input array.
 */
Array laplacian(const Array &array);

/**
 * @brief Generates a normal map from a given 2D array.
 *
 * This function calculates the normal vectors for each element in the 2D array
 * and returns them as a tensor. The normal map is commonly used in image
 * processing and 3D graphics to represent surface orientations based on height
 * values.
 *
 * @param  array A 2D array representing the height values from which normals
 *               are computed.
 * @return       A tensor containing the normal vectors for each position in the
 *               input array.
 */
Tensor normal_map(const Array &array);

/**
 * @brief Converts a normal map to a heightmap using direct summation of
 * gradients.
 *
 * This function computes a heightmap from a given normal map (`nmap`) by
 * integrating gradients derived from the normal map in two passes. The result
 * is the average of two independent integrations to reduce artifacts from
 * directional bias.
 *
 * @param  nmap A 3D tensor representing the normal map. It should have three
 *              channels (X, Y, Z) and a shape of (width, height, 3). The values
 *              are assumed to be in the range [0, 1].
 *
 * @return      An `Array` object representing the computed heightmap with the
 *              same spatial dimensions as the input normal map (width, height).
 *
 * @note
 * - The algorithm assumes the normal map values are normalized between 0 and 1.
 * - Two heightmaps (`z1` and `z2`) are computed using different traversal
 * orders, and the final result is their average to reduce directional bias.
 *
 * * **Example**
 * @include ex_normal_map_to_heightmap.cpp
 *
 * **Result**
 * @image html ex_normal_map_to_heightmap.png
 */
Array normal_map_to_heightmap(const Tensor &nmap);

/**
 * @brief Computes a phase field using spatially varying Gabor noise based on
 * the input heightmap.
 *
 * This function generates a 2D phase field by combining gradient angles from
 * the input array and Gabor noise, which is spatially distributed with varying
 * parameters. The resulting phase field can be used for procedural terrain
 * generation or other simulations.
 *
 * @param[in]  array          The input 2D heightmap array.
 * @param[in]  kw             Wave number for the Gabor kernel, determining the
 *                            frequency of the noise.
 * @param[in]  width          Width of the Gabor kernel.
 * @param[in]  seed           Random seed for reproducible Gabor noise
 *                            generation.
 * @param[in]  noise_amp      Noise amplitude added to the phase field.
 * @param[in]  prefilter_ir   Kernel radius for pre-smoothing the input array.
 *                            If negative, a default value is computed.
 * @param[in]  density_factor Factor controlling the density of the noise
 *                            points.
 * @param[in]  rotate90       Boolean flag to rotate the gradient angles by 90
 *                            degrees.
 * @param[out] p_gnoise_x     Optional pointer to store the generated Gabor
 *                            noise in the X direction.
 * @param[out] p_gnoise_y     Optional pointer to store the generated Gabor
 *                            noise in the Y direction.
 *
 * @return                    A 2D array representing the computed phase field.
 *
 * * **Example**
 * @include ex_phase_field.cpp
 *
 * **Result**
 * @image html ex_phase_field.png
 */
Array phase_field(const Array &array,
                  float        kw,
                  int          width,
                  uint         seed,
                  float        noise_amp = 0.f,
                  int          prefilter_ir = -1,
                  float        density_factor = 1.f,
                  bool         rotate90 = false,
                  Array       *p_gnoise_x = nullptr,
                  Array       *p_gnoise_y = nullptr);

/**
 * @brief Unwraps a 2D phase array to correct discontinuities in phase data.
 *
 * This function unwraps phase values in the given array, ensuring that
 * discontinuities exceeding \f$ \pi \f$ are adjusted to provide a continuous
 * phase result.
 *
 * @param  alpha A 2D array of wrapped phase values.
 * @return       A 2D array of unwrapped phase values.
 *
 * **Example**
 * @include ex_unwrap_phase.cpp
 *
 * **Result**
 * @image html ex_unwrap_phase.png
 */
Array unwrap_phase(const Array &alpha);

} // namespace hmap

namespace hmap::gpu
{

/*! @brief See hmap::gradient_norm */
Array gradient_norm(const Array &array);

} // namespace hmap::gpu