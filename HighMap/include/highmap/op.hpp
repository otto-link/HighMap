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
 * @brief Return the shaded relief map (or hillshading).
 *
 * @param z Input array.
 * @param azimuth Sun azimuth ('direction'), in degrees.
 * @param zenith Sun zenith ('elevation'), in degrees.
 * @param talus_ref Reference talus used to normalize gradient computations. May
 * be useful when working with true angles.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_hillshade.cpp
 *
 * **Result**
 * @image html ex_hillshade0.png
 * @image html ex_hillshade1.png
 *
 * @see {@link topographic_shading}
 */
Array hillshade(const Array &z,
                float        azimuth,
                float        zenith,
                float        talus_ref = 1.f);

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
 * @brief Synthesize a new heightmap based on an input array using a
 * non-parametric sampling method (very slow).
 *
 * See @cite Efros1999
 *
 * @param array Input array.
 * @param patch_shape Patch shape.
 * @param seed Random seed number.
 * @param error_threshold Error threshold for the patch selection process.
 * @return Synthetized array.
 *
 * **Example**
 * @include non_parametric_sampling.cpp
 *
 * **Result**
 * @image html non_parametric_sampling.png
 */
Array non_parametric_sampling(Array          &array,
                              hmap::Vec2<int> patch_shape,
                              uint            seed,
                              float           error_threshold = 0.1f);

/**
 * @brief Synthesize a new heightmap by stitching together small patches of the
 * input heightmaps.
 *
 * See @cite Efros2001.
 *
 * @param p_arrays Reference to the input arrays as a vector.
 * @param patch_base_shape Patch shape.
 * @param tiling Patch tiling.
 * @param overlap Patch overlap, in ]0, 1[.
 * @param seed Random seed number.
 * @param patch_flip Allow patch flipping (up-down and left-right).
 * @param patch_rotate Allow patch 90 degree rotation (square patches only).
 * @param patch_transpose Allow patch tranposition (square patches only).
 * @param filter_width_ratio Smooth filter width with respect the overlap
 * length.
 * @return Synthetized array (shape determined by patch shape and tiling
 * features).
 *
 * **Example**
 * @include ex_quilting.cpp
 *
 * **Result**
 * @image html ex_quilting0.png
 * @image html ex_quilting1.png
 * @image html ex_quilting2.png
 * @image html ex_quilting3.png
 * @image html ex_quilting4.png
 */
Array quilting(std::vector<Array *> p_arrays,
               hmap::Vec2<int>      patch_base_shape,
               hmap::Vec2<int>      tiling,
               float                overlap,
               uint                 seed,
               bool                 patch_flip = true,
               bool                 patch_rotate = true,
               bool                 patch_transpose = true,
               float                filter_width_ratio = 0.25f);

/**
 * @brief Synthesize a new heightmap by stitching together small patches from a
 * list of input heightmaps.
 *
 * @param array Reference to the input arrays.
 * @param patch_base_shape Patch shape.
 * @param overlap Patch overlap, in ]0, 1[.
 * @param seed Random seed number.
 * @param patch_flip Allow patch flipping (up-down and left-right).
 * @param patch_rotate Allow patch 90 degree rotation (square patches only).
 * @param patch_transpose Allow patch tranposition (square patches only).
 * @param filter_width_ratio Smooth filter width with respect the overlap
 * length.
 * @return Synthetized array.
 *
 * **Example**
 * @include ex_quilting.cpp
 *
 * **Result**
 * @image html ex_quilting0.png
 * @image html ex_quilting1.png
 * @image html ex_quilting2.png
 * @image html ex_quilting3.png
 * @image html ex_quilting4.png
 */
Array quilting_blend(std::vector<Array *> p_arrays,
                     hmap::Vec2<int>      patch_base_shape,
                     float                overlap,
                     uint                 seed,
                     bool                 patch_flip = true,
                     bool                 patch_rotate = true,
                     bool                 patch_transpose = true,
                     float                filter_width_ratio = 0.25f);

/**
 * @brief Synthesize a new heightmap by stitching together small patches of the
 * input heightmap. Wrapper to expand the input.
 *
 * @param array Input array.
 * @param expansion_ratio Determine the nex extent of the heightmap. If set to
 * 2, the heightmap is 2-times larger with features of the same "size".
 * @param patch_base_shape Patch shape.
 * @param overlap Patch overlap, in ]0, 1[.
 * @param seed Random seed number.
 * @param keep_input_shape If set to true, the output has the same shape as the
 * input.
 * @param patch_flip Allow patch flipping (up-down and left-right).
 * @param patch_rotate Allow patch 90 degree rotation (square patches only).
 * @param patch_transpose Allow patch tranposition (square patches only).
 * @param filter_width_ratio Smooth filter width with respect the overlap
 * length.
 * @return Synthetized array.
 *
 * **Example**
 * @include ex_quilting.cpp
 *
 * **Result**
 * @image html ex_quilting0.png
 * @image html ex_quilting1.png
 * @image html ex_quilting2.png
 * @image html ex_quilting3.png
 * @image html ex_quilting4.png
 */
Array quilting_expand(Array          &array,
                      float           expansion_ratio,
                      hmap::Vec2<int> patch_base_shape,
                      float           overlap,
                      uint            seed,
                      bool            keep_input_shape = false,
                      bool            patch_flip = true,
                      bool            patch_rotate = true,
                      bool            patch_transpose = true,
                      float           filter_width_ratio = 0.25f);

/**
 * @brief Synthesize a new heightmap by stitching together small patches of the
 * input heightmap. Wrapper to reshuffle an heightmap with the same shape as the
 * input.
 *
 * @param array Input array.
 * @param patch_base_shape Patch shape.
 * @param overlap Patch overlap, in ]0, 1[.
 * @param seed Random seed number.
 * @param patch_flip Allow patch flipping (up-down and left-right).
 * @param patch_rotate Allow patch 90 degree rotation (square patches only).
 * @param patch_transpose Allow patch tranposition (square patches only).
 * @param filter_width_ratio Smooth filter width with respect the overlap
 * length.
 * @return Synthetized array.
 *
 * **Example**
 * @include ex_quilting.cpp
 *
 * **Result**
 * @image html ex_quilting0.png
 * @image html ex_quilting1.png
 * @image html ex_quilting2.png
 * @image html ex_quilting3.png
 * @image html ex_quilting4.png
 */
Array quilting_shuffle(Array          &array,
                       hmap::Vec2<int> patch_base_shape,
                       float           overlap,
                       uint            seed,
                       bool            patch_flip = true,
                       bool            patch_rotate = true,
                       bool            patch_transpose = true,
                       float           filter_width_ratio = 0.25f);

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
 * @brief Apply the reverse midpoint displacement algorithm to the input array
 * (see @cite Belhadj2005).
 *
 * @param array Input array.
 * @param seed Random seed number.
 * @param noise_scale Noise scale amplitude.
 * @param threshold Theshold 'background' value.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_reverse_midpoint.cpp
 *
 * **Result**
 * @image html ex_reverse_midpoint.png
 */
Array reverse_midpoint(Array &array,
                       uint   seed,
                       float  noise_scale = 1.f,
                       float  threshold = 0.f);

/**
 * @brief Return the shadow intensity using a grid-based technic.
 *
 * @param z Input array.
 * @param shadow_talus Shadow talus.
 * @return Array Resulting array.
 */
Array shadow_grid(const Array &z, float shadow_talus);

/**
 * @brief Return the crude shadows from a height map.
 *
 * See https://www.shadertoy.com/view/Xlsfzl.
 *
 * @param z Input array.
 * @param azimuth Light azimuth ('direction'), in degress.
 * @param zenith Light zenith ('elevation'), in degrees.
 * @param distance Light distance.
 * @return Array Resulting array.
 */
Array shadow_heightmap(const Array &z,
                       float        azimuth = 180.f,
                       float        zenith = 45.f,
                       float        distance = 0.2f);

/**
 * @brief Return the topographic shadow intensity in [-1, 1].
 *
 * @param z Input array.
 * @param azimuth Sun azimuth ('direction').
 * @param zenith Sun zenith ('elevation').
 * @param talus_ref Reference talus used to normalize gradient computations. May
 * be useful when working with true angles.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_hillshade.cpp
 *
 * **Result**
 * @image html ex_hillshade0.png
 * @image html ex_hillshade1.png
 *
 * @see {@link hillshade}
 */
Array topographic_shading(const Array &z,
                          float        azimuth,
                          float        zenith,
                          float        talus_ref = 1.f);

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
