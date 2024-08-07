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
 * @brief Neighborhood lattice type.
 */
enum neighborhood : int
{
  MOORE,       ///< Moore
  VON_NEUMANN, ///< Von Neuman
  CROSS        ///< cross-shaped neighborhood (only diagonals)
};

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
 * @brief Set to zero any value lower than `vmin`.
 *
 * @param array Input array.
 * @param vmin Lower bound.
 */
void chop(Array &array, float vmin);

/**
 * @brief Set to zero any value lower than `vmax` and apply a linear decrease
 * slope between `vmax / 2` and `vmax`.
 *
 * @param array Input array.
 * @param vmax Upper bound.
 */
void chop_max_smooth(Array &array, float vmax);

/**
 * @brief Clamp array elements to a target range.
 *
 * @todo Smooth clamping.
 *
 * @param array Input array.
 * @param vmin Lower bound of the clamping range.
 * @param vmax Upper bound of the clamping range.
 *
 * **Example**
 * @include ex_clamp.cpp
 *
 * **Result**
 * @image html ex_clamp.png
 *
 * @see {@link remap}, {@link clamp_min}, {@link clamp_max}
 */
void clamp(Array &array, float vmin = 0, float vmax = 1);

/**
 * @brief Clamp array values lower than a given bound.
 *
 * @param array Input array.
 * @param vmin Lower bound.
 *
 * @see {@link clamp}, {@link clamp_max}
 */
void clamp_min(Array &array, float vmin);
void clamp_min(Array &array, const Array &vmin); ///< @overload

/**
 * @brief Clamp array values lower than a given bound with a smooth transition.
 *
 * @param array Input array.
 * @param vmin Lower bound.
 * @param k Smoothing parameter in [0, 1].
 */
void clamp_min_smooth(Array &array, float vmin, float k = 0.2f);

void clamp_min_smooth(Array       &array,
                      const Array &vmin,
                      float        k = 0.2f); ///< @overload

/**
 * @brief Clamp value lower than a given bound with a smooth transition.
 *
 * @param x Input value.
 * @param vmin Lower bound.
 * @param k Smoothing parameter in [0, 1].
 */
float clamp_min_smooth(float x, float vmin, float k = 0.2f);

/**
 * @brief Clamp array values larger than a given bound.
 *
 * @param array Input array.
 * @param vmax Upper bound.
 *
 * @see {@link clamp}, {@link clamp_min}
 */
void clamp_max(Array &array, float vmax);
void clamp_max(Array &array, const Array &vmax); ///< @overload

/**
 * @brief Clamp array values larger than a given bound with a smooth transition.
 *
 * @param array Input array.
 * @param vmax Upper bound.
 * @param k Smoothing parameter in [0, 1].
 */
void clamp_max_smooth(Array &array, float vmax, float k = 0.2f);

void clamp_max_smooth(Array       &array,
                      const Array &vmax,
                      float        k = 0.2f); ///< @overload

/**
 * @brief Clamp array values within a given interval with a smooth transition.
 *
 * @param array Input array.
 * @param vmin Lower bound.
 * @param vmax Upper bound.
 * @param k Smoothing parameter in [0, 1].
 */
void clamp_smooth(Array &array, float vmin, float vmax, float k = 0.2f);

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
 * @brief Apply histogram adjustement to the array values.
 *
 * @param array Input array.
 */
void equalize(Array &array);
void equalize(Array &array, Array *p_mask); ///< @overload

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
 * @brief Apply expanding, or "inflating", to emphasize the bulk of the terrain.
 *
 * @param array Input array.
 * @param ir Filter radius.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_expand.cpp
 *
 * **Result**
 * @image html ex_expand.png
 *
 * @see {@link ex_shrink}
 */
void expand(Array &array, int ir, Array *p_mask);
void expand(Array &array, int ir);                       ///< @overload
void expand(Array &array, Array &kernel);                ///< @overload
void expand(Array &array, Array &kernel, Array *p_mask); ///< @overload

/**
 * @brief Apply expanding, or "inflating", to emphasize the bulk of the terrain,
 * with a directional kernel.
 *
 * @param array Input array.
 * @param ir Filter radius.
 * @param angle Angle (in degrees).
 * @param aspect_ratio Pulse aspect ratio.
 * @param anisotropy Pulse width ratio between upstream and downstream sides.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_expand_directional.cpp
 *
 * **Result**
 * @image html ex_expand_directional.png
 */
void expand_directional(Array &array,
                        int    ir,
                        float  angle,
                        float  aspect_ratio,
                        float  anisotropy = 1.f,
                        Array *p_mask = nullptr);

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
 * @brief Return an heightmap retaining the main features of the input heightmap
 * but with a 'faceted' aspect.
 *
 * @param array Input arrau.
 * @param neighborhood Neighborhood type (see {@link neighborhood}).
 * @param p_noise_x, p_noise_y Reference to the input noise array used for
 * domain warping (NOT in pixels, with respect to a unit domain).
 * @return Array Output array.
 *
 * **Example**
 * @include ex_faceted.cpp
 *
 * **Result**
 * @image html ex_faceted.png
 */
Array faceted(const Array &array,
              int          neighborhood = 0,
              Array       *p_noise_x = nullptr,
              Array       *p_noise_y = nullptr);

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
 * @brief Fill values with a given downslope talus starting from the cell with
 * highest values.
 *
 * @param z Input array.
 * @param talus Downslope talus.
 * @param seed Random seed number.
 * @param noise_ratio Noise ratio (used to avoid grid orientation artifacts).
 *
 * **Example**
 * @include ex_fill_talus.cpp
 *
 * **Result**
 * @image html ex_fill_talus.png
 *
 * @see {@link thermal_scree}, {@link thermal_scree_fast}
 */
void fill_talus(Array &z, float talus, uint seed, float noise_ratio = 0.2f);

/**
 * @brief Fill values with a given downslope talus starting from the cell with
 * highest values, performed on a coarse mesh to optimize restitution time.
 *
 * @param z Input array.
 * @param shape_coarse  Array coarser shape used for the solver.
 * @param talus Downslope talus.
 * @param seed Random seed number.
 * @param noise_ratio Noise ratio (used to avoid grid orientation artifacts).
 *
 * **Example**
 * @include ex_fill_talus.cpp
 *
 * **Result**
 * @image html ex_fill_talus.png
 *
 * @see {@link thermal_scree}, {@link thermal_scree_fast}
 */
void fill_talus_fast(Array    &z,
                     Vec2<int> shape_coarse,
                     float     talus,
                     uint      seed,
                     float     noise_ratio = 0.2f);

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
 * @brief Apply a "folding" filter (successive absolute values) to the array
 * elements.
 * @param array Input array.
 * @param vmin Minimum reference value.
 * @param vmax Maximum reference value.
 * @param iterations Number of iterations.
 * @param k Absolute value smoothing parameters (expected > 0).
 *
 * **Example**
 * @include ex_fold.cpp
 *
 * **Result**
 * @image html ex_fold.png
 */
void fold(Array &array,
          float  vmin,
          float  vmax,
          int    iterations = 3,
          float  k = 0.05f);
void fold(Array &array, int iterations = 3, float k = 0.05f); ///< @overload

/**
 * @brief Apply a gain correction of the array elements.
 *
 * Gain correction is based on a power law.
 *
 * @param array Input array.
 * @param factor Gain factor (> 0).
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * **Example**
 * @include ex_gain.cpp
 *
 * **Result**
 * @image html ex_gain.png
 */
void gain(Array &array, float factor, Array *p_mask);

void gain(Array &array, float factor); ///< @overload

/**
 * @brief Apply gamma correction to the input array.
 *
 * @param array Input array.
 * @param gamma Gamma factor (> 0).
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * **Example**
 * @include ex_gamma_correction.cpp
 *
 * **Result**
 * @image html ex_gamma_correction.png
 */
void gamma_correction(Array &array, float gamma, Array *p_mask);

void gamma_correction(Array &array, float gamma); ///< @overload

void gamma_correction_thread(Array &array, float gamma);
void gamma_correction_xsimd(Array &array, float gamma);

/**
 * @brief Apply a "local" gamma correction to the input array.
 *
 * Values are normalized "locally" using a minimum and maximum filters before
 * applying the gamma correction.
 *
 * @param array Input array.
 * @param gamma Gamma factor (> 0).
 * @param ir Filter radius.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param k Smoothing factor.
 *
 * **Example**
 * @include ex_gamma_correction_local.cpp
 *
 * **Result**
 * @image html ex_gamma_correction_local.png
 */
void gamma_correction_local(Array &array, float gamma, int ir, float k = 0.1f);

void gamma_correction_local(Array &array,
                            float  gamma,
                            int    ir,
                            Array *p_mask,
                            float  k = 0.1f); ///< @overload

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
 * @brief Apply a low-pass Laplace filter.
 *
 * @param array Input array.
 * @param sigma Filtering intensity, in [0, 1].
 * @param iterations Number of iterations.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_laplace.cpp
 *
 * **Result**
 * @image html ex_laplace.png
 */
void laplace(Array &array, float sigma = 0.2f, int iterations = 3);

void laplace(Array &array,
             Array *p_mask,
             float  sigma = 0.2f,
             int    iterations = 3); ///< @overload

/**
 * @brief Apply a low-pass Laplace filter to a vector.
 *
 * @param v Input vector.
 * @param sigma Filtering intensity, in [0, 1].
 * @param iterations Number of iterations.
 */
void laplace1d(std::vector<float> &v, float sigma = 0.5f, int iterations = 1);

/**
 * @brief Apply a low-pass anisotropic Laplace filter.
 *
 * See https://en.wikipedia.org/wiki/Anisotropic_diffusion.
 *
 * @param array Input array.
 * @param talus Talus limit for edge sensitivity (gradient above this value are
 * less affected by the filering).
 * @param sigma Filtering intensity, in [0, 1].
 * @param iterations Number of iterations.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_laplace.cpp
 *
 * **Result**
 * @image html ex_laplace.png
 */
void laplace_edge_preserving(Array &array,
                             float  talus,
                             float  sigma = 0.2f,
                             int    iterations = 3);

void laplace_edge_preserving(Array &array,
                             float  talus,
                             Array *p_mask,
                             float  sigma = 0.2f,
                             int    iterations = 3);

/**
 * @brief Return the Laplacian of the input array.
 *
 * @param array Input array.
 * @return Array Laplacian array.
 */
Array laplacian(const Array &array);

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
 * @brief Apply a low-pass high-order filter (5, 7 or 9th-order).
 *
 * @param array Input array.
 * @param order Filter order (5, 7 or 9).
 * @param sigma Filtering intensity, in [0, 1].
 *
 * **Example**
 * @include ex_low_pass_high_order.cpp
 *
 * **Result**
 * @image html ex_low_pass_high_order.png
 *
 * @see {@link laplace}
 */
void low_pass_high_order(Array &array, int order = 9, float sigma = 1.f);

/**
 * @brief Apply of "binary" filter to the array values to get array values only
 * equal to 0 or 1.
 *
 * @param array Input array.
 * @param threshold Threshold value above which an array value is set to 1.
 */
void make_binary(Array &array, float threshold = 0.f);

void make_binary_xsimd(Array &array, float threshold = 0.f);

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
 * @brief Return the element-wise maximum of two arrays.
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @return Array Element-wise maximum array.
 */
Array maximum(const Array &array1, const Array &array2);
Array maximum(const Array &array1, const float value); ///< @overload

/**
 * @brief Return the 'local maxima' based on a maximum filter.
 *
 * @param array Input array.
 * @param ir Square kernel footprint radius.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_maximum_local.cpp
 *
 * **Result**
 * @image html ex_maximum_local.png
 *
 * @see {@link maximum_local_disk}, {@link minimum_local}
 */
Array maximum_local(const Array &array, int ir);

/**
 * @brief Return the 'local maxima' based on a maximum filter using a disk
 * kernel.
 *
 * @param array Input array.
 * @param ir Disk kernel footprint radius.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_maximum_local.cpp
 *
 * **Result**
 * @image html ex_maximum_local.png
 *
 * @see {@link maximum_local}, {@link minimum_local_disk}, {@link minimum_local}
 */
Array maximum_local_disk(const Array &array, int ir);

/**
 * @brief Return the polynomial cubic smooth element-wise maximum of two arrays.
 *
 * Smoothly blend the two arrays to get rid of the discontinuity of the
 * {@link minimum} function (see <a
 * href=https://iquilezles.org/articles/smin/>Inigo Quilez's articles</a>).
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @param k Smoothing parameter in [0, 1].
 * @return Array Element-wise smooth minimum between the two input arrays.
 *
 * @see {@link minimum_smooth}, {@link minimum}, {@link maximum}
 */
Array maximum_smooth(const Array &array1, const Array &array2, float k = 0.2);
float maximum_smooth(const float a,
                     const float b,
                     float       k = 0.2); ///< @overload

/**
 * @brief Transform input array elevation to match the histogram of a reference
 * array.
 *
 * @param array Input array.
 * @param array_reference Reference array.
 *
 * **Example**
 * @include ex_match_histogram.cpp
 *
 * **Result**
 * @image html ex_match_histogram.png
 */
void match_histogram(Array &array, const Array &array_reference);

/**
 * @brief Return the 'local mean' based on a mean filter.
 *
 * @param array Input array.
 * @param ir Square kernel footprint radius.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_mean_local.cpp
 *
 * **Result**
 * @image html ex_mean_local0.png
 * @image html ex_mean_local1.png
 *
 * @see {@link maximum_local}, {@link minimum_local}
 */
Array mean_local(const Array &array, int ir);

/**
 * @brief Apply a 3x3 median filter to the array.
 *
 * @param array Input array.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_median_3x3.cpp
 *
 * **Result**
 * @image html ex_median_3x3.png
 */
void median_3x3(Array &array, Array *p_mask);
void median_3x3(Array &array); ///< @overload

/**
 * @brief Return the element-wise minimum of two arrays.
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @return Array Element-wise minimum array.
 */
Array minimum(const Array &array1, const Array &array2);

Array minimum(const Array &array1, const float value); ///< @overload

/**
 * @brief Return the 'local minima' based on a maximum filter.
 *
 * @param array Input array.
 * @param ir Square kernel footprint radius.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_maximum_local.cpp
 *
 * **Result**
 * @image html ex_maximum_local.png
 *
 * @see {@link minimum_local}
 */
Array minimum_local(const Array &array, int ir);

/**
 * @brief Return the 'local minima' based on a maximum filter using a disk
 * kernel.
 *
 * @param array Input array.
 * @param ir Disk kernel footprint radius.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_maximum_local.cpp
 *
 * **Result**
 * @image html ex_maximum_local.png
 *
 * @see {@link maximum_local}, {@link maximum_local_disk}, {@link minimum_local}
 */
Array minimum_local_disk(const Array &array, int ir);

/**
 * @brief Return the polynomial cubic smooth element-wise minimum of two arrays.
 *
 * Smoothly blend the two arrays to get rid of the discontinuity of the
 * {@link minimum} function (see <a
 * href=https://iquilezles.org/articles/smin/>Inigo Quilez's articles</a>).
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @param k Smoothing parameter in [0, 1].
 * @return Array Element-wise smooth minimum between the two input arrays.
 *
 * @see {@link maximum_smooth}, {@link minimum}, {@link maximum}
 */
Array minimum_smooth(const Array &array1, const Array &array2, float k = 0.2);
float minimum_smooth(const float a, const float b, float k); ///< @overload

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
 * @brief Apply a displacement to the terrain along the normal direction.
 *
 * @param array Input array.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param amount Amount of displacement.
 * @param ir Pre-filtering radius.
 * @param reverse Reverse displacement direction.
 *
 * **Example**
 * @include ex_normal_displacement.cpp
 *
 * **Result**
 * @image html ex_normal_displacement.png
 */
void normal_displacement(Array &array,
                         float  amount = 0.1f,
                         int    ir = 0,
                         bool   reverse = false);

void normal_displacement(Array &array,
                         Array *p_mask,
                         float  amount = 0.1f,
                         int    ir = 0,
                         bool   reverse = false); ///< @overload

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
 * @brief Apply a "plateau-shape" filter to the array.
 *
 * @param array Input array.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param ir Plateau radius.
 * @param factor Gain factor (== plateau flatness).
 *
 * **Example**
 * @include ex_plateau.cpp
 *
 * **Result**
 * @image html ex_plateau.png
 */
void plateau(Array &array, Array *p_mask, int ir, float factor);
void plateau(Array &array, int ir, float factor); ///< @overload

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
 * @brief Transform heightmap to give a "billow" like appearance.
 *
 * @param array Input array.
 * @param vref Reference elevation where the elevation is folded.
 * @param k Smoothing coefficient.
 *
 * **Example**
 * @include ex_recast.cpp
 *
 * **Result**
 * @image html ex_recast.png
 */
void recast_billow(Array &array, float vref, float k);

/**
 * @brief Transform heightmap to give a "canyon" like appearance.
 *
 * @param array Input array.
 * @param vcut Canyon top elevation.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param gamma Gamma factor (> 0) (@see {@link gamma_correction}).
 *
 * **Example**
 * @include ex_recast.cpp
 *
 * **Result**
 * @image html ex_recast.png
 */
void recast_canyon(Array &array, const Array &vcut, float gamma = 4.f);

void recast_canyon(Array       &array,
                   const Array &vcut,
                   Array        p_mask,
                   float        gamma = 4.f); ///< @overload

void recast_canyon(Array &array,
                   float  vcut,
                   Array *p_mask,
                   float  gamma = 4.f,
                   Array *p_noise = nullptr); ///< @overload

void recast_canyon(Array &array,
                   float  vcut,
                   float  gamma = 4.f,
                   Array *p_noise = nullptr); ///< @overload

/**
 * @brief Transform heightmap to add cliffs where gradient are steep enough.
 *
 * @param array Input array.
 * @param talus Reference talus.
 * @param ir Fiter radius
 * @param amplitude Cliff amplitude.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param gain Gain of the gain filter (i.e. cliff steepness).
 *
 * **Example**
 * @include ex_recast.cpp
 *
 * **Result**
 * @image html ex_recast.png
 */
void recast_cliff(Array &array,
                  float  talus,
                  int    ir,
                  float  amplitude,
                  float  gain = 2.f);

void recast_cliff(Array &array,
                  float  talus,
                  int    ir,
                  float  amplitude,
                  Array *p_mask,
                  float  gain = 2.f); ///< @overload

void recast_cliff_directional(Array &array,
                              float  talus,
                              int    ir,
                              float  amplitude,
                              float  angle,
                              float  gain = 2.f); ///< @overloads

void recast_cliff_directional(Array &array,
                              float  talus,
                              int    ir,
                              float  amplitude,
                              float  angle,
                              Array *p_mask,
                              float  gain = 2.f); ///< @overloads

/**
 * @brief Transform heightmap to give a "peak" like appearance.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array Input array.
 * @param ir Filter radius.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param gamma Gamma factor (> 0) (@see {@link gamma_correction}).
 * @param k Smoothing parameter (> 0).
 *
 * **Example**
 * @include ex_recast.cpp
 *
 * **Result**
 * @image html ex_recast.png
 */
void recast_peak(Array &array, int ir, float gamma = 2.f, float k = 0.1f);

void recast_peak(Array &array,
                 int    ir,
                 Array *p_mask,
                 float  gamma = 2.f,
                 float  k = 0.1f); ///< @overload

/**
 * @brief Transform heightmap by adding "rock-like" features at higher slopes.
 *
 * @param array Input array.
 * @param talus Talus limit.
 * @param ir Filter radius.
 * @param amplitude Filter amplitude.
 * @param seed Random number seed.
 * @param kw Noise wavenumber with respect to a unit domain (for rocks).
 * @param p_mask Filter mask, expected in [0, 1].
 * @param gamma Gamma correction coefficent (for rocks)
 * @param p_noise Reference to the input noise used for rock features (overrides
 * default generator).
 *
 * **Example**
 * @include ex_recast.cpp
 *
 * **Result**
 * @image html ex_recast.png
 */
void recast_rocky_slopes(Array      &array,
                         float       talus,
                         int         ir,
                         float       amplitude,
                         uint        seed,
                         float       kw,
                         float       gamma = 0.5f,
                         Array      *p_noise = nullptr,
                         Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

void recast_rocky_slopes(Array      &array,
                         float       talus,
                         int         ir,
                         float       amplitude,
                         uint        seed,
                         float       kw,
                         Array      *p_mask,
                         float       gamma = 0.5f,
                         Array      *p_noise = nullptr,
                         Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Transform heightmap to give a "cliff" like appearance.
 *
 * @param array Input array.
 * @param vref Reference elevation where the elevation is folded.
 * @param k Smoothing coefficient.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_recast.cpp
 *
 * **Result**
 * @image html ex_recast.png
 */
void recast_sag(Array &array, float vref, float k);

void recast_sag(Array &array,
                float  vref,
                float  k,
                Array *p_mask); ///< @overload

/**
 * @brief Apply a curve adjustment filter to the array.
 *
 * The curve is monotically interpolated and values outside the input range are
 * clipped.
 *
 * @param array Input array.
 * @param t Input value of the correction curve.
 * @param v Output value of the correction curve.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_recurve.cpp
 *
 * **Result**
 * @image html ex_recurve.png
 */
void recurve(Array                    &array,
             const std::vector<float> &t,
             const std::vector<float> &v);

void recurve(Array                    &array,
             const std::vector<float> &t,
             const std::vector<float> &v,
             Array                    *p_mask);

/**
 * @brief Apply a curve adjustment filter using a "bumpy exponential-shape"
 * curve.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array Input array.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param tau Exponential decay.
 *
 * **Example**
 * @include ex_recurve_xxx.cpp
 *
 * **Result**
 * @image html ex_recurve_xxx.png
 */
void recurve_bexp(Array &array, float tau = 0.5f);

void recurve_bexp(Array &array, Array *p_mask, float tau = 0.5f);

/**
 * @brief Apply a curve adjustment filter using a "sharp exponential-shape"
 * curve.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array Input array.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param tau Exponential decay.
 *
 * **Example**
 * @include ex_recurve_xxx.cpp
 *
 * **Result**
 * @image html ex_recurve_xxx.png
 */
void recurve_exp(Array &array, float tau = 0.5f);

void recurve_exp(Array &array, Array *p_mask, float tau = 0.5f);

/**
 * @brief Apply a curve adjustment filter using Kumaraswamy's cumulative
 * distribution function.
 *
 * @param array Input array.
 * @param a 'Parameter a', drives curve shape close to `0`.
 * @param b 'Parameter b', drives curve shape close to `1`.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_recurve_xxx.cpp
 *
 * **Result**
 * @image html ex_recurve_xxx.png
 */
void recurve_kura(Array &array, float a, float b);

void recurve_kura(Array &array, float a, float b, Array *p_mask);

/**
 * @brief Apply a curve adjustment filter using a smooth "S-shape" curve.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array Input array.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_recurve_xxx.cpp
 *
 * **Result**
 * @image html ex_recurve_xxx.png
 */
void recurve_s(Array &array);

void recurve_s(Array &array, Array *p_mask);

/**
 * @brief Apply a curve adjustment filter using a nth-order smoothstep curve.
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * @param array Input array.
 * @param n Smoothstep order (in [0, inf[).
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_recurve_xxx.cpp
 *
 * **Result**
 * @image html ex_recurve_xxx.png
 */
void recurve_smoothstep_rational(Array &array, float n);

void recurve_smoothstep_rational(Array &array, float n, Array *p_mask);

/**
 * @brief Remap array elements from a starting range to a target range.
 *
 * By default the starting range is taken to be [min(), max()] of the input
 * array.
 *
 * @param array Input array.
 * @param vmin The lower bound of the range to remap to.
 * @param vmax The lower bound of the range to remap to.
 * @param from_min The lower bound of the range to remap from.
 * @param from_max The upper bound of the range to remap from.
 *
 *  * **Example**
 * @include ex_remap.cpp
 *
 * @see {@link clamp}
 */
void remap(Array &array,
           float  vmin,
           float  vmax,
           float  from_min,
           float  from_max);

void remap(Array &array, float vmin = 0, float vmax = 1); ///< @overload

/**
 * @brief Remap array elements from a starting range to a target range.
 *
 * @param array Input array.
 * @param scaling Amplitude scaling.
 * @param vref Reference 'zero' value.
 */
void rescale(Array &array, float scaling, float vref = 0.f);

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
 * @brief Saturate the array values based on the input interval [vmin, vmax]
 * (the output amplitude is not modified).
 *
 * @param array Input array.
 * @param vmin The lower bound of the range to remap to.
 * @param vmax The lower bound of the range to remap to.
 * @param from_min The lower bound of the range to remap from.
 * @param from_max The upper bound of the range to remap from.
 *
 * **Example**
 * @include ex_saturate.cpp
 *
 * **Result**
 * @image html ex_saturate.png
 */
void saturate(Array &array,
              float  vmin,
              float  vmax,
              float  from_min,
              float  from_max,
              float  k);

void saturate(Array &array, float vmin, float vmax,
              float k); ///< @overload

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
 * @brief Apply sharpening filter (based on Laplace operator).
 *
 * @param array Input array.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param ratio Output ratio between sharpened (ratio = 1) and non-sharpened
 * array (ratio = 0).
 *
 * **Example**
 * @include ex_sharpen.cpp
 *
 * **Result**
 * @image html ex_sharpen.png
 */
void sharpen(Array &array, float ratio = 1.f);
void sharpen(Array &array, Array *p_mask, float ratio = 1.f); ///< @overload

/**
 * @brief Apply sharpening filter (based on a smooth_cone filter).
 *
 * @param array Input array.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param ir Filter radius.
 * @param scale Sharpening scale.
 *
 * **Example**
 * @include ex_sharpen_cone.cpp
 *
 * **Result**
 * @image html ex_sharpen_cone.png
 */
void sharpen_cone(Array &array, int ir, float intensity = 0.5f);
void sharpen_cone(Array &array,
                  Array *p_mask,
                  int    ir,
                  float  scale = 0.5f); ///< @overload

/**
 * @brief Apply shrinking, or "deflating", to emphasize the ridges.
 *
 * @param array Input array.
 * @param ir Filter radius.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_expand.cpp
 *
 * **Result**
 * @image html ex_expand.png
 *
 * @see {@link ex_expand}
 */
void shrink(Array &array, int ir);
void shrink(Array &array, int ir, Array *p_mask);        ///< @overload
void shrink(Array &array, Array &kernel);                ///< @overload
void shrink(Array &array, Array &kernel, Array *p_mask); ///< @overload

/**
 * @brief Apply shrinking, or "deflating", to emphasize the ridges of the
 * terrain, with a directional kernel.
 *
 * @param array Input array.
 * @param ir Filter radius.
 * @param angle Angle (in degrees).
 * @param aspect_ratio Pulse aspect ratio.
 * @param anisotropy Pulse width ratio between upstream and downstream sides.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_expand_directional.cpp
 *
 * **Result**
 * @image html ex_expand_directional.png
 */
void shrink_directional(Array &array,
                        int    ir,
                        float  angle,
                        float  aspect_ratio,
                        float  anisotropy = 1.f,
                        Array *p_mask = nullptr);

/**
 * @brief Apply filtering to the array using convolution with a cone kernel.
 *
 * @param array Input array.
 * @param ir Cone radius (half-width is half this radius).
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_smooth_cone.cpp
 *
 * **Result**
 * @image html ex_smooth_cone.png
 */
void smooth_cone(Array &array, int ir);
void smooth_cone(Array &array, int ir, Array *p_mask); ///< @overload

/**
 * @brief Apply filtering to the array using convolution with a cubic pulse.
 *
 * Can be used as an alternative (with a much smaller support) to Gaussian
 * smoothing. For direct comparison with Gaussian smoothing, 'ir' needs to be
 * twice larger.
 *
 * @param array Input array.
 * @param ir Pulse radius (half-width is half this radius).
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_smooth_cpulse.cpp
 *
 * **Result**
 * @image html ex_smooth_cpulse.png
 *
 * @see {@link smooth_gaussian}
 */
void smooth_cpulse(Array &array, int ir);
void smooth_cpulse(Array &array, int ir, Array *p_mask); ///< @overload

/**
 * @brief Apply Gaussian filtering to the array.
 *
 * @param array Input array.
 * @param ir Gaussian half-width.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_smooth_gaussian.cpp
 *
 * **Result**
 * @image html ex_smooth_gaussian.png
 */
void smooth_gaussian(Array &array, int ir);
void smooth_gaussian(Array &array, int ir, Array *p_mask); ///< @overload

/**
 * @brief Apply cubic pulse smoothing to fill lower flat regions while
 * preserving some sharpness.
 *
 * "Cheap" version of {@link thermal_auto_bedrock}.
 *
 * @param array Input array.
 * @param ir Pulse radius.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param k Transition smoothing parameter in [0, 1].
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 * output field.
 *
 * **Example**
 * @include ex_smooth_fill.cpp
 *
 * **Result**
 * @image html ex_smooth_fill.png
 *
 * @see {@link smooth_cpulse}, {@link thermal_auto_bedrock}
 */
void smooth_fill(Array &array,
                 int    ir,
                 float  k = 0.1f,
                 Array *p_deposition_map = nullptr);

void smooth_fill(Array &array,
                 int    ir,
                 Array *p_mask,
                 float  k = 0.1f,
                 Array *p_deposition_map = nullptr); ///< @overload

/**
 * @brief Apply smoothing to fill holes (elliptic concave surfaces).
 *
 * @param array Input array.
 * @param ir Filter radius.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_smooth_fill_holes.cpp
 *
 * **Result**
 * @image html ex_smooth_fill_holes.png
 *
 * @see {@link smooth_smear_peaks}
 */
void smooth_fill_holes(Array &array, int ir);
void smooth_fill_holes(Array &array, int ir, Array *p_mask); ///< @overload

/**
 * @brief Apply smoothing to smear peaks (elliptic convexe surfaces).
 *
 * @param array Input array.
 * @param ir Filter radius.
 * @param p_mask Filter mask, expected in [0, 1].
 *
 * **Example**
 * @include ex_smooth_fill_holes.cpp
 *
 * **Result**
 * @image html ex_smooth_fill_holes.png
 *
 * @see {@link smooth_fill_holes}
 */
void smooth_fill_smear_peaks(Array &array, int ir);
void smooth_fill_smear_peaks(Array &array,
                             int    ir,
                             Array *p_mask); ///< @overload

/**
 * @brief Steepen (or flatten) the array map.
 *
 * @param array Input array.
 * @param scale Filter amplitude.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param ir Filtering radius of the array gradients.
 *
 * **Example**
 * @include ex_steepen.cpp
 *
 * **Result**
 * @image html ex_steepen.png
 */
void steepen(Array &array, float scale, int ir = 8);
void steepen(Array &array,
             float  scale,
             Array *p_mask,
             int    ir = 8); ///< @overload

/**
 * @brief Steepen array values by applying a nonlinear convection operator in a
 * given direction (see to Burger's equarion for instance).
 *
 * @todo verify results.
 *
 * @param array Input array (elements expected to be in [-1, 1]).
 * @param angle Steepening direction (in degrees).
 * @param p_mask Filter mask, expected in [0, 1].
 * @param iterations Number of iterations.
 * @param ir Smoothing radius of the array values before differentiation.
 * @param dt "Time step", can be chosen smaller than 1 for fine tuning of the
 * steepening effect.
 *
 * **Example**
 * @include ex_steepen_convective.cpp
 *
 * **Result**
 * @image html ex_steepen_convective.png
 */
void steepen_convective(Array &array,
                        float  angle,
                        int    iterations = 1,
                        int    ir = 0,
                        float  dt = 0.1f);

void steepen_convective(Array &array,
                        float  angle,
                        Array *p_mask,
                        int    iterations = 1,
                        int    ir = 0,
                        float  dt = 0.1f); ///< @overload

/**
 * @brief Use symmetry for to fill values at the domain borders, over a given
 * buffer depth.
 *
 * @param array Input array.
 * @param buffer_sizes Buffer size at the borders {east, west, south, north}.
 */
void sym_borders(Array &array, Vec4<int> buffer_sizes);

/**
 * @brief Return the transposed array.
 * @param array Input array.
 * @return Transposed array.
 */
Array transpose(const Array &array);

/**
 * @brief
 *
 * @param array Input array.
 * @param seed Random seed number.
 * @param node_density Node density (as a ratio to the number of cells of the
 * input array).
 * @param p_weight Reference to the density distribution array, expected in [0,
 * 1].
 * @return Array Output array.
 *
 * **Example**
 * @include ex_tessellate.cpp
 *
 * **Result**
 * @image html ex_tessellate.png
 */
Array tessellate(Array &array,
                 uint   seed,
                 float  node_density = 0.001f,
                 Array *p_weight = nullptr);

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
 * @brief
 *
 * @param array Input array.
 * @param wrinkle_amplitude Effect amplitude.
 * @param p_mask Filter mask, expected in [0, 1].
 * @param displacement_amplitude Drives wrinkles primitive displacement.
 * @param ir Smooth filter radius.
 * @param kw Underlying primitive wavenumber.
 * @param seed Underlying primitive random seed number.
 * @param octaves Underlying primitive number of octaves.
 * @param weight Underlying primitive weight.
 *
 * **Example**
 * @include ex_wrinkle.png
 *
 * **Result**
 * @image html ex_wrinkle.cpp
 */
void wrinkle(Array      &array,
             float       wrinkle_amplitude,
             Array      *p_mask,
             float       displacement_amplitude = 1.f,
             int         ir = 0,
             float       kw = 2.f,
             uint        seed = 1,
             int         octaves = 8,
             float       weight = 0.7f,
             Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

void wrinkle(Array      &array,
             float       wrinkle_amplitude,
             float       displacement_amplitude = 1.f,
             int         ir = 0,
             float       kw = 2.f,
             uint        seed = 1,
             int         octaves = 8,
             float       weight = 0.7f,
             Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f}); ///< @overload

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
