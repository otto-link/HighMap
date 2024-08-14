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
#include "highmap/geometry.hpp"
#include "highmap/math.hpp"

namespace hmap
{

/**
 * @brief Point-wise alteration: locally enforce a new elevation value while
    maintaining the 'shape' of the heightmap.
 *
 * @param array Input array.
 * @param cloud Cloud object, defining alteration coordinates and elevation
 variations.
 * @param ir Alteration kernel minimal radius.
 * @param footprint_ratio Defined how the radius is scales with variation
 intensity (the greater the larger the alterations footprint)
 * @param shift Noise shift {xs, ys} for each directions, with respect to a
 * unit domain.
 * @param scale Domain scaling, in [0, 1].
 *
 * **Example**
 * @include ex_alter_elevation.cpp
 *
 * **Result**
 * @image html ex_alter_elevation.png
 */
void alter_elevation(Array      &array,
                     Cloud      &cloud,
                     int         ir,
                     float       footprint_ratio = 1.f,
                     Vec2<float> shift = {0.f, 0.f},
                     Vec2<float> scale = {1.f, 1.f});

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
 * @brief Return the 'exclusion' blending of two arrays.
 *
 * See for instance https://en.wikipedia.org/wiki/Blend_modes.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array1 1st array.
 * @param array2 2nd array.
 * @return Array Reference to the resulting object.
 *
 * **Example**
 * @include ex_blend.cpp
 *
 * **Result**
 * @image html ex_blend0.png
 * @image html ex_blend1.png
 * @image html ex_blend2.png
 * @image html ex_blend3.png
 * @image html ex_blend4.png
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 * {@link blend_soft}
 */
Array blend_exclusion(const Array &array1, const Array &array2);

/**
 * @brief Return the blending of two arrays based on their gradients.
 *
 * @param array1 1st array.
 * @param array2 2nd array.
 * @param ir Filtering radius (in pixels).
 * @return Array Reference to the resulting object.
 */
Array blend_gradients(const Array &array1, const Array &array2, int ir = 4);

/**
 * @brief Return the 'negate' blending of two arrays.
 *
 * @param array1 1st array.
 * @param array2 2nd array.
 * @return Array Reference to the resulting object.
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 * {@link blend_soft}
 */
Array blend_negate(const Array &array1, const Array &array2);

/**
 * @brief Return the 'overlay' blending of two arrays.
 *
 * See for instance https://en.wikipedia.org/wiki/Blend_modes.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array1 1st array.
 * @param array2 2nd array.
 * @return Array Reference to the resulting object.
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 * {@link blend_soft}
 */
Array blend_overlay(const Array &array1, const Array &array2);

/**
 * @brief Return the 'soft' blending of two arrays.
 *
 * Based on <a
 * href=http://www.pegtop.net/delphi/articles/blendmodes/softlight.htm>Pegtop
 * soft light mode</a>.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array1 1st array.
 * @param array2 2nd array.
 * @return Array Reference to the resulting object.
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 * {@link blend_soft}
 */
Array blend_soft(const Array &array1, const Array &array2);

/**
 * @brief Apply a closing algorithm to the input array using a square
 * structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array closing(const Array &array, int ir);

/**
 * @brief Return the convolution product of the array with a 1D kernel (row, 'i'
 * direction).
 *
 * @param array Input array.
 * @param kernel Kernel (1D).
 * @return Array Convolution result.
 *
 * **Example**
 * @include ex_convolve1d_ij.cpp
 *
 * **Result**
 * @image html ex_convolve1d_ij.png
 *
 * @see {@link convolve1d_j}
 */
Array convolve1d_i(const Array &array, const std::vector<float> &kernel);

/**
 * @brief Return the convolution product of the array with a 1D kernel (column,
 * 'j' direction).
 *
 * @param array Input array.
 * @param kernel Kernel (1D).
 * @return Array Convolution result.
 *
 * **Example**
 * @include ex_convolve1d_ij.cpp
 *
 * **Result**
 * @image html ex_convolve1d_ij.png
 *
 * @see {@link convolve1d_i}
 */
Array convolve1d_j(const Array &array, const std::vector<float> &kernel);

/**
 * @brief Return the convolution product of the array with a given kernel. The
 * output has the same shape as the input (symmetry boundary conditions).
 *
 * @param array Input array.
 * @param kernel Kernel array.
 * @return Array Convolution result.
 *
 * **Example**
 * @include ex_convolve2d_svd.cpp
 */
Array convolve2d(const Array &array, const Array &kernel);

/**
 * @brief Return the convolution product of the array with a given kernel. The
 * output has a smaller size than the input.
 *
 * @param array Input array.
 * @param kernel Kernel array.
 * @return Array Convolution result (shape: {array.shape[0] - kernel.shape[0],
 * array.shape[1] - kernel.shape[1]}).
 */
Array convolve2d_truncated(const Array &array, const Array &kernel);

/**
 * @brief Return the approximate convolution product of the array with a
 * Singular Value Decomposition (SVD) of a kernel.
 *
 * See reference @cite McGraw2014 and this post
 * https://bartwronski.com/2020/02/03/separate-your-filters-svd-and-low-rank-approximation-of-image-filters/
 *
 * @param z Input array.
 * @param kernel Kernel array.
 * @param rank Approximation rank: the first 'rank' singular values/vectors are
 * used to approximate the convolution product.
 * @return Array Convolution result.
 *
 * **Example**
 * @include ex_convolve2d_svd.cpp
 *
 * **Result**
 * @image html ex_convolve2d_svd.png
 */
Array convolve2d_svd(const Array &z, const Array &kernel, int rank = 3);

/**
 * @brief Return the approximate convolution product of the array with a
 * Singular Value Decomposition (SVD) of a kernel combined with a rotation of
 * the kernel.
 *
 * @param z Input array.
 * @param kernel Kernel array.
 * @param rank Approximation rank: the first 'rank' singular values/vectors are
 * used to approximate the convolution product.
 * @param n_rotations Number of kernel rotations.
 * @param seed Random seed number.
 * @return Array Convolution result.
 *
 * **Example**
 * @include ex_convolve2d_svd.cpp
 *
 * **Result**
 * @image html ex_convolve2d_svd.png
 */
Array convolve2d_svd_rotated_kernel(const Array &z,
                                    const Array &kernel,
                                    int          rank = 3,
                                    int          n_rotations = 6,
                                    uint         seed = 1);

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
 * @brief Apply a dilation algorithm to the input array using a square
 * structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array dilation(const Array &array, int ir);

/**
 * @brief Return the Euclidean distance transform.
 *
 * Exact transform based on Meijster et al. algorithm @cite Meijster2000.
 *
 * @param array Input array to be transformed, will be converted into binary: 1
 * wherever input is greater than 0, 0 elsewhere.
 * @param return_squared_distance Wheter the distance returned is squared or
 * not.
 * @return Array Reference to the output array.
 *
 * **Example**
 * @include ex_distance_transform.cpp
 *
 * **Result**
 * @image html ex_distance_transform0.png
 * @image html ex_distance_transform1.png
 */
Array distance_transform(const Array &array,
                         bool         return_squared_distance = false);

/**
 * @brief Apply an erosion algorithm to the input array using a square
 * structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 *  **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array erosion(const Array &array, int ir);

/**
 * @brief Linear extrapolation of values at the borders (i = 0, j = 0, ...)
 * based on inner values.
 *
 * @param array Input array.
 * @param nbuffer Buffer depth.
 * @param sigma Relaxation coefficient.
 *
 * @see {@link fill_borders}
 *
 */
void extrapolate_borders(Array &array, int nbuffer = 1, float sigma = 0.f);

/**
 * @brief
 * @param array
 * @param strength
 * @param dist_fct
 * @param p_noise
 * @param bbox
 *
 * **Example**
 * @include ex_falloff.cpp
 *
 * **Result**
 * @image html ex_falloff.png
 */
void falloff(Array           &array,
             float            strength = 1.f,
             DistanceFunction dist_fct = DistanceFunction::EUCLIDIAN,
             Array           *p_noise = nullptr,
             Vec4<float>      bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Fill values at the borders (i = 0, j = 0, ...) based on 1st neighbor
 * values.
 *
 * @param array Input array.
 *
 * @see {@link extrapolate_borders}
 */
void fill_borders(Array &array);

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
 * @brief Apply a flood fill algorithm to the input array.
 *
 * @param array Input array.
 * @param i Seed point index.
 * @param j Seed point index.
 * @param fill_value Filling value.
 * @param background_value Background value.
 *
 * **Example**
 * @include ex_flood_fill.cpp
 *
 * **Result**
 * @image html ex_flood_fill.png
 */
void flood_fill(Array &array,
                int    i,
                int    j,
                float  fill_value = 1.f,
                float  background_value = 0.f);

/**
 * @brief Return an array with buffers at the boundaries (values filled by
 * symmetry).
 *
 * @param array Input array.
 * @param buffers Buffer size {east, west, south, north}.
 * @param zero_padding Use zero-padding instead of symmetry to fill values.
 * @return Array New array with buffers.
 */
Array generate_buffered_array(const Array &array,
                              Vec4<int>    buffers,
                              bool         zero_padding = false);

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
 * @brief Make the array periodic in both directions.
 *
 * @param array Input array.
 * @param nbuffer Transition width at the boundaries.
 *
 * **Example**
 * @include ex_make_periodic.cpp
 *
 * **Result**
 * @image html ex_make_periodic0.png
 * @image html ex_make_periodic1.png
 */
void make_periodic(Array &array, int nbuffer);

/**
 * @brief Make the array periodic in both directions using a stitching operation
 * minimizing errors.
 *
 * @param array Input array.
 * @param Overlap overlap based on domain half size (if overlap = 1, the
 * transition, made on both sides, then spans the whole domain).
 * @return Array Periodic array.
 *
 * **Example**
 * @include ex_make_periodic_stitching.cpp
 *
 * **Result**
 * @image html ex_make_periodic_stitching0.png
 * @image html ex_make_periodic_stitching1.png
 */
Array make_periodic_stitching(Array &array, float overlap);

/**
 * @brief
 *
 * @param array Input array.
 * @param Overlap overlap based on domain half size (if overlap = 1, the
 * transition, made on both sides, then spans the whole domain).
 * @param tiling Array tiling.
 * @return Tiled array.
 *
 * **Example**
 * @include make_periodic_tiling.cpp
 *
 * **Result**
 * @image html make_periodic_tiling.png
 */
Array make_periodic_tiling(Array &array, float overlap, Vec2<int> tiling);

/**
 * @brief Return the mixing of a set of arrays based on a parameter `t`.
 *
 * @warning Values of array `t` are expected to be in [0, 1].
 *
 * @param t Mixing coefficient, define locally, in [0, 1].
 * @param arrays References to the input arrays.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_mixer.cpp
 *
 * **Result**
 * @image html ex_mixer.png
 */
Array mixer(const Array &t, const std::vector<Array *> arrays);

/**
 * @brief Apply a morphological_black_hat algorithm to the input array using a
 * square structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array morphological_black_hat(const Array &array, int ir);

/**
 * @brief Apply a morphological_gradient algorithm to the input array using a
 * square structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array morphological_gradient(const Array &array, int ir);

/**
 * @brief Apply a morphological_top_hat algorithm to the input array using a
 * square structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array morphological_top_hat(const Array &array, int ir);

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
 * @brief Apply an opening algorithm to the input array using a square
 * structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array opening(const Array &array, int ir);

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
 * @brief Enforce values at the boundaries of the array.
 *
 * @param array Input array.
 * @param values Value at the borders {east, west, south, north}.
 * @param buffer_sizes Buffer size at the borders {east, west, south, north}.
 *
 * **Example**
 * @include ex_set_borders.cpp
 *
 * **Result**
 * @image html ex_set_borders.png
 */
void set_borders(Array      &array,
                 Vec4<float> border_values,
                 Vec4<int>   buffer_sizes);

void set_borders(Array &array,
                 float  border_values,
                 int    buffer_sizes); ///< @overload

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
 * @brief Use symmetry for to fill values at the domain borders, over a given
 * buffer depth.
 *
 * @param array Input array.
 * @param buffer_sizes Buffer size at the borders {east, west, south, north}.
 */
void sym_borders(Array &array, Vec4<int> buffer_sizes);

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
 * @brief Return the transposed array.
 * @param array Input array.
 * @return Transposed array.
 */
Array transpose(const Array &array);

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
 * @brief Fill values at the borders (i = 0, j = 0, ...) with zeros.
 *
 * @param array Input array.
 *
 * @see {@link fill_borders}
 */
void zeroed_borders(Array &array);

/**
 * @brief Apply a smooth transition to zero at the array borders.
 *
 * @param array Input array.
 * @param sigma Transition half-width ratio.
 * @param p_noise Reference to the input noise arrays.
 * @param bbox Domain bounding box.
 *
 * **Example**
 * @include ex_zeroed_edges.cpp
 *
 * **Result**
 * @image html ex_zeroed_edges.png
 */
void zeroed_edges(Array           &array,
                  float            sigma = 1.f,
                  DistanceFunction dist_fct = DistanceFunction::EUCLIDIAN,
                  Array           *p_noise = nullptr,
                  Vec4<float>      bbox = {0.f, 1.f, 0.f, 1.f});

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
