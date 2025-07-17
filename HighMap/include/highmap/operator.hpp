/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file op.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Provides various operations for manipulating arrays, including kernel
 * addition, interpolation, detrending, and more.
 *
 * This header file defines functions for performing operations on arrays, such
 * as adding kernels, interpolation, detrending, stacking arrays, and
 * inpainting. It also includes functions for generating random vectors, filling
 * arrays using functions, and manipulating patches.
 *
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once

#include "highmap/array.hpp"
#include "highmap/math.hpp"

namespace hmap
{

/**
 * @brief Add a kernel to a specified position in an array.
 *
 * This function adds the values of a smaller kernel array to the input array,
 * centered at the specified indices (i, j). The kernel is added element-wise.
 *
 * @param array  The input array to which the kernel is added.
 * @param kernel The kernel array to be added.
 * @param i      The row index in the input array where the kernel is centered.
 * @param j      The column index in the input array where the kernel is
 *               centered.
 */
void add_kernel(Array &array, const Array &kernel, int i, int j);

/**
 * @brief Adds a smoothed maximum value from a kernel to a specified position in
 * a 2D array.
 *
 * This function applies a truncated kernel to the given array at position (ic,
 * jc), where the kernel is truncated to ensure it fits within the array's
 * bounds. The maximum value between the current array value and the kernel
 * value is computed using a smoothing factor, and the result is stored back in
 * the array.
 *
 * @param array    Reference to the 2D array (heightmap) where the kernel is
 *                 applied.
 * @param kernel   The kernel array containing values to be applied to the
 *                 array.
 * @param k_smooth The smoothing factor used in the maximum_smooth function.
 * @param ic       The x-coordinate of the center in the array where the kernel
 *                 is applied.
 * @param jc       The y-coordinate of the center in the array where the kernel
 *                 is applied.
 *
 * The kernel is truncated to ensure it fits within the bounds of the array.
 * After applying the kernel, the values in the array are updated with the
 * result of the maximum_smooth function.
 */
void add_kernel_maximum_smooth(Array       &array,
                               const Array &kernel,
                               float        k_smooth,
                               int          i,
                               int          j);

/**
 * @brief Apply linear regression for detrending of a 2D array.
 *
 * This function performs detrending on the input array by applying linear
 * regression separately to each row and column, removing trends from the data.
 *
 * @param  array Input 2D array to be detrended.
 * @return       Array The detrended output array.
 *
 * **Example**
 * @include ex_detrend.cpp
 *
 * **Result**
 * @image html ex_detrend.png
 */
Array detrend_reg(const Array &array);

/**
 * @brief Horizontally stack two arrays side by side.
 *
 * This function concatenates two arrays along their columns, forming a new
 * array with the columns of the first array followed by the columns of the
 * second array.
 *
 * @param  array1 The first array to stack.
 * @param  array2 The second array to stack.
 * @return        Array The resulting array obtained by horizontally stacking
 * `array1` and `array2`.
 */
Array hstack(const Array &array1, const Array &array2);

/**
 * @brief Perform diffusion-based inpainting to fill a specified region of an
 * array.
 *
 * This function fills the region defined by a mask in the input array using
 * diffusion-based inpainting, which propagates known values to missing regions.
 *
 * @param  array      Input array with missing regions.
 * @param  mask       Mask specifying the region to be inpainted.
 * @param  iterations Number of diffusion iterations to perform.
 * @return            Array The array with the inpainted region.
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
 * @brief Generate a vector of evenly spaced numbers over a specified interval.
 *
 * This function creates a vector containing evenly spaced values from `start`
 * to `stop`. The number of values is specified by `num`, and `endpoint`
 * determines whether to include the end value in the output.
 *
 * @see             linspace_jitted
 *
 * @param  start    Starting value of the interval.
 * @param  stop     End value of the interval.
 * @param  num      Number of values to generate.
 * @param  endpoint If true, include the end value in the output vector.
 * @return          std::vector<float> Vector of evenly spaced values.
 */
std::vector<float> linspace(float start,
                            float stop,
                            int   num,
                            bool  endpoint = true);

/**
 * @brief Generate a vector of jittered (noised) numbers over a specified
 * interval.
 *
 * This function creates a vector with values spaced over an interval but with
 * added noise, controlled by the `ratio` parameter. The noise is applied to an
 * evenly spaced grid. The `seed` parameter controls the randomness.
 *
 * @see             linspace
 *
 * @param  start    Starting value of the interval.
 * @param  stop     End value of the interval.
 * @param  num      Number of values to generate.
 * @param  ratio    Jittering ratio applied to the evenly spaced grid.
 * @param  seed     Random seed for generating jittered values.
 * @param  endpoint If true, include the end value in the output vector.
 * @return          std::vector<float> Vector of jittered values.
 */
std::vector<float> linspace_jitted(float start,
                                   float stop,
                                   int   num,
                                   float ratio,
                                   int   seed,
                                   bool  endpoint = true);

/**
 * @brief Generate a vector filled with random values within a specified range.
 *
 * This function creates a vector with random values uniformly distributed
 * between `min` and `max`. The number of values and the randomness is
 * controlled by the `seed` parameter.
 *
 * @param  min  Lower bound of the random values.
 * @param  max  Upper bound of the random values.
 * @param  num  Number of random values to generate.
 * @param  seed Random seed for generating values.
 * @return      std::vector<float> Vector of random values.
 */
std::vector<float> random_vector(float min, float max, int num, int seed);

/**
 * @brief Fill an array using a scalar function based on (x, y) coordinates.
 *
 * This function populates an array with values computed from a scalar function
 * that depends on (x, y) coordinates. Additional input arrays can affect the
 * function's computation, such as control parameters, noise, and stretching.
 *
 * @param array        The array to be filled with computed values.
 * @param bbox         The bounding box of the domain specified as {xmin, xmax,
 *                     ymin, ymax}.
 * @param p_ctrl_param Pointer to an array of control parameters affecting the
 *                     scalar function.
 * @param p_noise_x    Pointer to an array of noise values along the x-direction
 *                     for domain warping.
 * @param p_noise_y    Pointer to an array of noise values along the y-direction
 *                     for domain warping.
 * @param p_stretching Pointer to an array of local wavenumber multipliers for
 *                     adjusting the function.
 * @param fct_xy       The scalar function to compute values at (x, y) with an
 *                     initial value.
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
    const Array                              *p_ctrl_param,
    const Array                              *p_noise_x,
    const Array                              *p_noise_y,
    const Array                              *p_stretching,
    std::function<float(float, float, float)> fct_xy);

/**
 * @brief Fill an array using a scalar function based on (x, y) coordinates with
 * subsampling.
 *
 * This function is similar to the one above but includes a `subsampling`
 * parameter to optimize performance. The array is subsampled during computation
 * based on the `subsampling` factor, which determines how frequently the scalar
 * function is applied.
 *
 * @param array        The array to be filled with computed values.
 * @param bbox         The bounding box of the domain specified as {xmin, xmax,
 *                     ymin, ymax}.
 * @param p_ctrl_param Pointer to an array of control parameters affecting the
 *                     scalar function.
 * @param p_noise_x    Pointer to an array of noise values along the x-direction
 *                     for domain warping.
 * @param p_noise_y    Pointer to an array of noise values along the y-direction
 *                     for domain warping.
 * @param p_stretching Pointer to an array of local wavenumber multipliers for
 *                     adjusting the function.
 * @param fct_xy       The scalar function to compute values at (x, y) with an
 *                     initial value.
 * @param subsampling  The factor by which the array is subsampled during
 *                     computation.
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
    const Array                              *p_ctrl_param,
    const Array                              *p_noise_x,
    const Array                              *p_noise_y,
    const Array                              *p_stretching,
    std::function<float(float, float, float)> fct_xy,
    int                                       subsampling); ///< @overload

/**
 * @brief Find the vertical cut path with the minimum cost using Dijkstra's
 * algorithm.
 *
 * This function identifies the vertical cut path in an array from the bottom to
 * the top that has the minimum cumulative cost. The path is determined using
 * Dijkstra's algorithm.
 *
 * @param error  Input array containing error or cost values.
 * @param path_i Output vector of indices representing the cut path.
 */
void find_vertical_cut_path(const Array &error, std::vector<int> &path_i);

/**
 * @brief Generate a smooth mask based on a cut path.
 *
 * This function creates a mask based on the vertical cut path indices obtained
 * from the `find_vertical_cut_path` function. The mask is smoothed using a
 * specified filtering radius.
 *
 * @param  shape      Shape of the mask to be generated.
 * @param  cut_path_i Vector of vertical cut path indices.
 * @param  ir         Filtering radius for smoothing the mask.
 * @return            Array The generated smooth mask.
 */
Array generate_mask(hmap::Vec2<int> shape, std::vector<int> cut_path_i, int ir);

/**
 * @brief Extracts a random sub-array (patch) from the input array, with
 * optional transformations.
 *
 * This function selects a random patch from the specified input array and
 * applies optional transformations such as flipping, rotation, and
 * transposition. Additionally, if a list of secondary arrays is provided,
 * corresponding patches are extracted and transformed in the same way.
 *
 * @param  array               The main input array from which the patch is
 *                             extracted.
 * @param  patch_shape         The shape (dimensions) of the patch to be
 *                             extracted.
 * @param  gen                 Random number generator for selecting patch
 *                             location.
 * @param  patch_flip          If true, allows the patch to be flipped
 *                             vertically or horizontally.
 * @param  patch_rotate        If true, allows the patch to be rotated by 90
 *                             degrees.
 * @param  patch_transpose     If true, allows the patch to be transposed.
 * @param  p_secondary_arrays  Optional pointer to a list of secondary arrays.
 *                             If provided, patches will be extracted and
 *                             transformed from each array in the list, with the
 *                             same transformations as the primary patch.
 * @param  p_secondary_patches Optional pointer to a list for storing the
 *                             transformed patches from each secondary array.
 *
 * @return                     The extracted and transformed patch from the main
 *                             input array.
 */
Array get_random_patch(const Array          &array,
                       hmap::Vec2<int>       patch_shape,
                       std::mt19937         &gen,
                       bool                  patch_flip = false,
                       bool                  patch_rotate = false,
                       bool                  patch_transpose = false,
                       std::vector<Array *> *p_secondary_arrays = nullptr,
                       std::vector<Array>   *p_secondary_patches = nullptr);

/**
 * @brief Vertically stack two arrays.
 *
 * This function concatenates two arrays along their rows, resulting in a new
 * array where the rows of the second array are stacked below the rows of the
 * first array.
 *
 * @param  array1 The first array to be stacked.
 * @param  array2 The second array to be stacked.
 * @return        Array The resulting array with `array1` stacked on top of
 *                `array2`.
 */
Array vstack(const Array &array1, const Array &array2);

} // namespace hmap
