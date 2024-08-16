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
#include "highmap/math.hpp"

namespace hmap
{

/**
 * @brief Return the bilinear interpolated value based on four input values.
 *
 * @param f00 Value at (u, v) = (0, 0).
 * @param f10 Value at (u, v) = (1, 0).
 * @param f01 Value at (u, v) = (0, 1).
 * @param f11 Value at (u, v) = (1, 1).
 * @param u 'u' interpolation parameter, expected to be in [0, 1[.
 * @param v 'v' interpolation parameter, expected to be in [0, 1[.
 * @return float
 */
inline float bilinear_interp(float f00,
                             float f10,
                             float f01,
                             float f11,
                             float u,
                             float v)
{
  float a10 = f10 - f00;
  float a01 = f01 - f00;
  float a11 = f11 - f10 - f01 + f00;
  return f00 + a10 * u + a01 * v + a11 * u * v;
}

/**
 * @brief Simplified detrending for array using linear regression applied to
 * columns and rows.
 * @param array Input array.
 * @return Output array.
 *
 * **Example**
 * @include ex_detrend.cpp
 *
 * **Result**
 * @image html ex_detrend.png
 */
Array detrend_reg(const Array &array);

/**
 * @brief Apply diffusion-based inpainting to fill a region (defined by mask) of
 * the input array (@cite Oliveira2001).
 *
 * @param array Input array.
 * @param mask Mask, region to be inpainted.
 * @param iterations Number of diffusion iterations.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_inpainting_diffusion.cpp
 *
 * **Result**
 * @image html ex_inpainting_diffusion.png
 */
Array inpainting_diffusion(const Array &array,
                           const Array &mask,
                           int          iterations);

/**
 * @brief Return evenly spaced numbers over a specified interval.
 *
 * @see linspace_jittered
 *
 * @param start Starting value.
 * @param stop End value.
 * @param num Number of values.
 * @param endpoint Include or not the end point in the output vector.
 * @return std::vector<float> Values.
 */
std::vector<float> linspace(float start,
                            float stop,
                            int   num,
                            bool  endpoint = true);

/**
 * @brief Return noised spaced numbers over a specified interval.
 *
 * @see linspace
 *
 * @param start Starting value.
 * @param stop End value.
 * @param num Number of values.
 * @param ratio Jittering ratio with respect to an evenly spaced grid.
 * @param seed Random seed number.
 * @param endpoint Include or not the end point in the output vector.
 * @return std::vector<float> Values
 */
std::vector<float> linspace_jitted(float start,
                                   float stop,
                                   int   num,
                                   float ratio,
                                   int   seed,
                                   bool  endpoint = true);

/**
 * @brief Generate a vector filled with random values.
 *
 * @param min Lower bound of random distribution.
 * @param max Upper bound of random distribution.
 * @param num Number of values.
 * @param seed Random seed number.
 * @return std::vector<float>
 */
std::vector<float> random_vector(float min, float max, int num, int seed);

// --- helpers

/**
 * @brief Among all the possible cut paths in an array from bottom to top and
 * find the one with the minimum cost using Dijkstra's algorithm
 *
 * @param error Input array.
 * @param path_i Indices of the path.
 */
void find_vertical_cut_path(Array &error, std::vector<int> &path_i);

/**
 * @brief Generate a smooth mask based on an input cut path (@see
 * find_vertical_cut_path).
 *
 * @param shape Mask shape
 * @param cut_path_i Vertica cut path indices
 * @param ir Filtering radius.
 */
Array generate_mask(hmap::Vec2<int> shape, std::vector<int> cut_path_i, int ir);

} // namespace hmap
