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
#include "highmap/math.hpp"

namespace hmap
{

/**
 * @brief Adds a kernel (with a smaller shape) to a given array, centered on
 * indices (i, j).
 *
 * @param array The input Array.
 * @param kernel The kernel to add.
 * @param i The row index in the input array where the kernel is added.
 * @param j The column index in the input array where the kernel is added.
 */
void add_kernel(Array &array, const Array &kernel, int i, int j);

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
 * @brief Horizontally stacks two arrays in sequence (column-wise).
 *
 *        This function concatenates two arrays side by side (column-wise) to
 * form a new array. The resulting array has the columns of the first array
 * followed by the columns of the second array.
 *
 * @param array1 The first array to stack.
 * @param array2 The second array to stack.
 * @return Array The resulting array obtained by horizontally stacking
 * `array1` and `array2`.
 */
Array hstack(const Array &array1, const Array &array2);

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

/**
 * @brief Fills an array using a scalar function based on (x, y) coordinates.
 *
 *        This function fills the specified array with values computed from a
 * scalar function that depends on (x, y) coordinates. The function takes into
 * account domain warping and local wavenumber multipliers through additional
 * input arrays. The function `fct_xy` is applied to each point within the
 * domain defined by `bbox`.
 *
 * @param array The array to be filled with computed values.
 * @param bbox The bounding box of the domain, specified as {xmin, xmax, ymin,
 * ymax}.
 * @param p_ctrl_param Pointer to an array of control parameters, affecting
 * the scalar function.
 * @param p_noise_x Pointer to an array of noise values along the x-direction
 * for domain warping.
 * @param p_noise_y Pointer to an array of noise values along the y-direction
 * for domain warping.
 * @param p_stretching Pointer to an array of local wavenumber multipliers for
 * adjusting the function.
 * @param fct_xy The scalar function to compute values at (x, y) with an
 * initial value.
 *
 * **Example**
 * @include ex_fill_array_using_xy_function.cpp
 *
 * **Result**
 * @image html ex_fill_array_using_xy_function.png
 */
void fill_array_using_xy_function(
    Array                                    &array,
    Vec4<float>                               bbox,
    Array                                    *p_ctrl_param,
    Array                                    *p_noise_x,
    Array                                    *p_noise_y,
    Array                                    *p_stretching,
    std::function<float(float, float, float)> fct_xy);

/**
 * @brief Fills an array using a scalar function based on (x, y) coordinates
 * with subsampling.
 *
 *        This function is similar to the one above but includes an additional
 * `subsampling` parameter. It fills the specified array with values computed
 * from a scalar function while considering subsampling for performance
 * optimization. The `subsampling` factor determines how frequently the
 * function is applied across the array.
 *
 * @param array The array to be filled with computed values.
 * @param bbox The bounding box of the domain, specified as {xmin, xmax, ymin,
 * ymax}.
 * @param p_ctrl_param Pointer to an array of control parameters, affecting
 * the scalar function.
 * @param p_noise_x Pointer to an array of noise values along the x-direction
 * for domain warping.
 * @param p_noise_y Pointer to an array of noise values along the y-direction
 * for domain warping.
 * @param p_stretching Pointer to an array of local wavenumber multipliers for
 * adjusting the function.
 * @param fct_xy The scalar function to compute values at (x, y) with an
 * initial value.
 * @param subsampling The factor by which the array is subsampled during
 * computation.
 *
 * **Example**
 * @include ex_fill_array_using_xy_function.cpp
 *
 * **Result**
 * @image html ex_fill_array_using_xy_function.png
 */
void fill_array_using_xy_function(
    Array                                    &array,
    Vec4<float>                               bbox,
    Array                                    *p_ctrl_param,
    Array                                    *p_noise_x,
    Array                                    *p_noise_y,
    Array                                    *p_stretching,
    std::function<float(float, float, float)> fct_xy,
    int                                       subsampling); ///< @overload

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

/**
 * @brief Extract a random patch from the given input array.
 *
 *        This function samples a random sub-array (patch) from the provided
 * input array. The patch is extracted according to the specified shape and can
 * be optionally flipped, rotated, or transposed. The randomness is controlled
 * by a pseudo-random number generator.
 *
 * @param array The input array from which the patch is extracted.
 * @param patch_shape The dimensions of the patch to be extracted.
 * @param gen A pseudo-random number generator used to ensure randomness.
 * @param patch_flip If true, the patch may be flipped horizontally or
 * vertically with a 50% probability for each flip direction.
 * @param patch_rotate If true, the patch may be rotated by 90 degrees with a
 * 50% probability.
 * @param patch_transpose If true, the patch may be transposed (swapped rows and
 * columns) with a 50% probability.
 * @return Array The randomly sampled patch from the input array, potentially
 * modified by flipping, rotating, or transposing.
 */
Array get_random_patch(Array          &array,
                       hmap::Vec2<int> patch_shape,
                       std::mt19937   &gen,
                       bool            patch_flip = false,
                       bool            patch_rotate = false,
                       bool            patch_transpose = false);

/**
 * @brief Return stacked arrays in sequence vertically (row-wise).
 *
 *        This function concatenates two arrays along their vertical axis
 * (row-wise) to produce a new array. The resulting array has the rows of the
 * second array stacked below the rows of the first array.
 *
 * @param array1 The first array to be stacked.
 * @param array2 The second array to be stacked.
 * @return Array A new array consisting of `array1` stacked vertically on top
 * of `array2`.
 */
Array vstack(const Array &array1, const Array &array2);

} // namespace hmap
