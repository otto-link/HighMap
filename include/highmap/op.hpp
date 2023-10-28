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

namespace hmap
{

/**
 * @brief Return the absolute value of the array elements.
 *
 * @param array Input array.
 * @return Array Output array.
 */
Array abs(const Array &array);

/**
 * @brief Return the smooth absolute value of the array elements.
 *
 * @param array Input array.
 * @param k Smoothing coefficient.
 * @return Array Output array.
 */
Array abs_smooth(const Array &array, float mu);

/**
 * @brief Apply the almost unit identity function.
 *
 * Function that maps the unit interval to itself with zero derivative at 0 and
 * "one" derivative at 1 (see <a
 * href=https://iquilezles.org/articles/functions/>Inigo Quilez's articles</a>).
 *
 * @param array Input array.
 */
void almost_unit_identity(Array &array);

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
 * @brief Return the approximate hypothenuse of two numbers.
 *
 * @param a a
 * @param b a
 * @return float ~sqrt(a**2 + b**2)
 */
inline float approx_hypot(float a, float b)
{
  a = std::abs(a);
  b = std::abs(b);
  if (a > b)
    std::swap(a, b);
  return 0.414 * a + b;
}

/**
 * @brief Return the approximate inverse square root of a number.
 *
 * @param a a
 * @return float ~1/sqrt(a)
 */
inline float approx_rsqrt(float a)
{
  union
  {
    float    f;
    uint32_t i;
  } conv = {.f = a};
  conv.i = 0x5f3759df - (conv.i >> 1);
  conv.f *= 1.5F - (a * 0.5F * conv.f * conv.f);
  return conv.f;
}

/**
 * @brief Return the arctan of the array elements.
 *
 * @param array Input array.
 * @return Array Reference to the current object.
 */
Array atan(const Array &array);

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
 * @brief Return the connected-component labelling of the array.
 *
 * See https://en.wikipedia.org/wiki/Connected-component_labeling.
 *
 * @param array Input array.
 * @param surface_threshold Surface threshold (in number of pixels): if not set
 * to zero, components with a surface smaller than the threshold are removed.
 * @param background_value Background value.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_connected_components.cpp
 *
 * **Result**
 * @image html ex_connected_components0.png
 * @image html ex_connected_components1.png
 */
Array connected_components(const Array &array,
                           float        surface_threshold = 0.f,
                           float        background_value = 0.f);

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
 * @brief Return the cosine of the array elements.
 *
 * @param array Input array.
 * @return Array Reference to the current object.
 */
Array cos(const Array &array);

/**
 * @brief Return the Gaussian curvature @cite Kurita1992.
 *
 * @param z Input array.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_curvature_gaussian.cpp
 *
 * **Result**
 * @image html ex_curvature_gaussian0.png
 * @image html ex_curvature_gaussian1.png
 * @image html ex_curvature_gaussian2.png
 */
Array curvature_gaussian(const Array &z);

/**
 * @brief Return the mean curvature @cite Kurita1992.
 *
 * @param z Input array.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_curvature_gaussian.cpp
 *
 * **Result**
 * @image html ex_curvature_gaussian0.png
 * @image html ex_curvature_gaussian1.png
 * @image html ex_curvature_gaussian2.png
 */
Array curvature_mean(const Array &z);

/**
 * @brief Return the Euclidean distance transform.
 *
 * Exact transform based on Meijster et al. algorithm @cite Meijster2000.
 *
 * @param array Input array to be transformed, will be converted into binary: 1
 * wherever input is greater than 0, 0 elsewhere.
 * @return Array Reference to the output array.
 *
 * **Example**
 * @include ex_distance_transform.cpp
 *
 * **Result**
 * @image html ex_distance_transform0.png
 * @image html ex_distance_transform1.png
 */
Array distance_transform(const Array &array);

/**
 * @brief Apply histogram adjustement to the array values.
 *
 * @param array Input array.
 */
void equalize(Array &array);

void equalize(Array &array, Array *p_mask); ///< @overload

/**
 * @brief Return the exponantial of the array elements.
 *
 * @param array Input array.
 * @return Array Reference to the current object.
 */
Array exp(const Array &array);

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
 *
 * @see {@link fill_borders}
 *
 */
void extrapolate_borders(Array &array, int nbuffer = 1);

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
 * @return Array New array with buffers.
 */
Array generate_buffered_array(const Array &array, Vec4<int> buffers);

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
 * @return Array Gradient norm.
 *
 * **Example**
 * @include ex_gradient_norm.cpp
 *
 * **Result**
 * @image html ex_gradient_norm.png
 */
Array gradient_norm(const Array &array);

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
 * @brief Return the square root of the sum of the squares of the two input
 * arrays.
 *
 * @relates Map
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @return Array Hypothenuse.
 */
Array hypot(const Array &array1, const Array &array2);

/**
 * @brief Return the labelling of each cell of the array based on a k-means
 * clustering, with two arrays of input data.
 *
 * @param array1 Input array #1.
 * @param array2 Input array #2.
 * @param nclusters Number of clusters.
 * @param seed Random seed number.
 * @return Array Resulting label array.
 *
 * **Example**
 * @include ex_kmeans_clustering.cpp
 *
 * **Result**
 * @image html ex_kmeans_clustering0.png
 * @image html ex_kmeans_clustering1.png
 */
Array kmeans_clustering2(const Array &array1,
                         const Array &array2,
                         int          nclusters,
                         uint         seed = 1);

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
 * @brief Return the linear interpolation between two arrays by a parameter t.
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @param t Interpolation parameter (in [0, 1]).
 * @return Array Interpolated array.
 */
Array lerp(const Array &array1, const Array &array2, const Array &t);

Array lerp(const Array &array1, const Array &array2, float t); ///< @overload

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
 * @brief Return the log10 of the array elements.
 *
 * @param array Input array.
 * @return Array Reference to the current object.
 */
Array log10(const Array &array);

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
 *
 * Tiled
 * @image html ex_make_periodic1.png
 */
void make_periodic(Array &array, int nbuffer);

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

/**
 * @brief Return the mixing of a set of arrays based on a parameter `t`.
 *
 * @warning Values of array `t` are expected to be in [0, 1].
 *
 * @param t Mixing coefficient, define locally, in [0, 1].
 * @param arrays Input arrays.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_mixer.cpp
 *
 * **Result**
 * @image html ex_mixer.png
 */
Array mixer(const Array t, const std::vector<Array> arrays);

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
 * @brief Return the array elements raised to the power 'exp'.
 *
 * @param exp Exponent.
 * @return Array Reference to the current object.
 */
Array pow(const Array &array, float exp);

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
void recast_rocky_slopes(Array &array,
                         float  talus,
                         int    ir,
                         float  amplitude,
                         uint   seed,
                         float  kw,
                         float  gamma = 0.5f,
                         Array *p_noise = nullptr);

void recast_rocky_slopes(Array &array,
                         float  talus,
                         int    ir,
                         float  amplitude,
                         uint   seed,
                         float  kw,
                         Array *p_mask,
                         float  gamma = 0.5f,
                         Array *p_noise = nullptr);

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
 * @brief Return the relative elevation within a a radius `ir`.
 *
 * @param array Input array.
 * @param ir Lookup radius, in pixels.
 * @return Array Relative elevation, in [0, 1].
 *
 * **Example**
 * @include relative_elevation.cpp
 *
 * **Result**
 * @image html relative_elevation.png
 */
Array relative_elevation(const Array &array, int ir);

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
 * @brief Rotate the array.
 *
 * @param array Input array.
 * @param angle Rotation angle in degrees.
 *
 * **Example**
 * @include ex_rotate.cpp
 *
 * **Result**
 * @image html ex_rotate.png
 */
void rotate(Array &array, float angle);

/**
 * @brief Return rugosity estimate (based on the skewness).
 *
 * @param z Input array.
 * @param ir Square kernel footprint radius.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_rugosity.cpp
 *
 * **Result**
 * @image html ex_rugosity0.png
 * @image html ex_rugosity1.png
 */
Array rugosity(const Array &z, int ir);

/**
 * @brief Return blob detection using the Laplacian of Gaussian (LoG) approach.
 *
 * @param array Input array.
 * @param ir Kernel radius.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select_blob.cpp
 *
 * **Result**
 * @image html ex_select_blob0.png
 * @image html ex_select_blob1.png
 */
Array select_blob_log(const Array &array, int ir);

/**
 * @brief Return holes or bumps detection based on the mean curvature of the
 * heightmap.
 *
 * @param array Input array.
 * @param ir Kernel radius.
 * @param concave Select 'holes' if set to true, and select 'bumps' if set to
 * false.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select_cavities.cpp
 *
 * **Result**
 * @image html ex_select_cavities.png
 */
Array select_cavities(const Array &array, int ir, bool concave = true);

/**
 * @brief Return an array with elements equal to 1 where input elements are
 * equal to `value`.
 *
 * @param array Input array.
 * @param value Criteria value.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select.cpp
 *
 * **Result**
 * @image html ex_select0.png
 * @image html ex_select1.png
 * @image html ex_select2.png
 * @image html ex_select3.png
 * @image html ex_select4.png
 */
Array select_eq(const Array &array, float value);

/**
 * @brief Return an array with elements equal to 1 where input elements are
 * larger than `value`.
 *
 * @param array Input array.
 * @param value Criteria value.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select.cpp
 *
 * **Result**
 * @image html ex_select0.png
 * @image html ex_select1.png
 * @image html ex_select2.png
 * @image html ex_select3.png
 * @image html ex_select4.png
 */
Array select_gt(const Array &array, float value);

/**
 * @brief Return an array with elements equal to 1 where input elements are
 * within the bounds provided.
 *
 * @param array Input array.
 * @param value1 Lower bound.
 * @param value2 Upper bound.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select.cpp
 *
 * **Result**
 * @image html ex_select0.png
 * @image html ex_select1.png
 * @image html ex_select2.png
 * @image html ex_select3.png
 * @image html ex_select4.png
 */
Array select_interval(const Array &array, float value1, float value2);

/**
 * @brief Return an array with elements equal to 1 where input elements are
 * smaller than `value`.
 *
 * @param array Input array.
 * @param value Criteria value.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_select.cpp
 *
 * **Result**
 * @image html ex_select0.png
 * @image html ex_select1.png
 * @image html ex_select2.png
 * @image html ex_select3.png
 * @image html ex_select4.png
 */
Array select_lt(const Array &array, float value);

/**
 * @brief Return an array weighted by the gap between the gradient angle and a
 * given angle.
 *
 * @param array Input array.
 * @param angle Reference angle.
 * @return Array Output array.
 */
Array select_gradient_angle(const Array &array, float angle);

/**
 * @brief Return an array filled with 1 where the gradient is larger than a
 * given value and 0 elsewhere.
 *
 * @param array Input array.
 * @param talus_center Reference talus.
 * @return Array Output array.
 */
Array select_gradient_binary(const Array &array, float talus_center);

/**
 * @brief Return an array weighted (exponantial decay) by the gradient norm of
 * the input array.
 *
 * @param array Input array.
 * @param talus_center Reference talus.
 * @param talus_sigma Talus half-width.
 * @return Array Output array.
 */
Array select_gradient_exp(const Array &array,
                          float        talus_center,
                          float        talus_sigma);

/**
 * @brief Return an array weighted (square inverse) by the gradient norm of the
 * input array.
 *
 * @param array Input array.
 * @param talus_center Reference talus.
 * @param talus_sigma Talus half-width.
 * @return Array Output array.
 */
Array select_gradient_inv(const Array &array,
                          float        talus_center,
                          float        talus_sigma);

/**
 * @brief Return an array filled with 1 at the blending transition between two
 * arrays, and 0 elsewhere.
 *
 * @param array1 Input array 1.
 * @param array2 Input array 2.
 * @param array_blend Blending of array 1 and 2 to analyze.
 * @return Array Resulting array
 *
 * **Example**
 * @include ex_select_transitions.cpp
 *
 * **Result**
 * @image html ex_select_transitions0.png
 * @image html ex_select_transitions1.png
 * @image html ex_select_transitions2.png
 */
Array select_transitions(const Array &array1,
                         const Array &array2,
                         const Array &array_blend);

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

void sharpen(Array &array, Array *p_mask, float ratio = 1.f);

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
 * @brief Return the sine of the array elements.
 *
 * @param array Input array.
 * @return Array Reference to the current object.
 */
Array sin(const Array &array);

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

void smooth_cpulse(Array &array, int ir, Array *p_mask);

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

void smooth_gaussian(Array &array, int ir, Array *p_mask);

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
                 Array *p_deposition_map = nullptr);

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

void smooth_fill_smear_peaks(Array &array, int ir, Array *p_mask);

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

void steepen(Array &array, float scale, Array *p_mask, int ir = 8);

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
                        float  dt = 0.1f);

/**
 * @brief Use symmetry for to fill values at the domain borders, over a given
 * buffer depth.
 *
 * @param array Input array.
 * @param buffer_sizes Buffer size at the borders {east, west, south, north}.
 */
void sym_borders(Array &array, Vec4<int> buffer_sizes);

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
 * @brief Return the "valley width", corresponding to the distance to the
 * concave region frontier (in this concave frontier).
 *
 * @param z Input array.
 * @param ir Filter radius.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_valley_width.cpp
 *
 * **Result**
 * @image html ex_valley_width0.png
 * @image html ex_valley_width1.png
 */
Array valley_width(const Array &z, int ir = 0);

/**
 * @brief Apply a warping effect to the array.
 *
 * @todo test higher order interpolations
 *
 * @param array Input array.
 * @param p_dx Reference to the translation array definition following 'i'
 * direction, (scale is 1:1, dx = 1 => 1 pixel).
 * @param p_dy Reference to the translation array definition following 'j'
 * direction, (scale is 1:1, dy = 1 => 1 pixel).
 * @param scale Scaling applied to dx and and dy.
 *
 * **Example**
 * @include ex_warp.cpp
 *
 * **Result**
 * @image html ex_warp.png
 */
void warp(Array       &array,
          const Array *p_dx,
          const Array *p_dy,
          float        scale = 1.f);

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
                      float  amount = 1.f,
                      int    ir = 4,
                      bool   reverse = false);

void warp_directional(Array &array,
                      float  angle,
                      Array *p_mask,
                      float  amount = 1.f,
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
                    float  amount = 1.f,
                    int    ir = 4,
                    bool   reverse = false);

void warp_downslope(Array &array,
                    Array *p_mask,
                    float  amount = 1.f,
                    int    ir = 4,
                    bool   reverse = false); ///< @overload

/**
 * @brief Apply a warping effect to the array with displacements based on fbm
 * noise.
 *
 * @param array Input array.
 * @param scale Warping scale (scale is 1:1, scale = 1 => 1 pixel).
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 * @param octaves Number of octaves.
 * @param shift Noise shift {xs, ys} for each directions, with respect to a unit
 * domain.
 */
void warp_fbm(Array      &array,
              float       scale,
              Vec2<float> kw,
              uint        seed,
              int         octaves = 8,
              Vec2<float> shift = {0.f, 0.f});

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
 * @param p_noise Reference to the input noise array used for domain warping
 * (NOT in pixels, with respect to a unit domain).
 * @param shift Shift {xs, ys} for each directions, with respect to a unit
 * domain.
 * @param scale Domain relative scale.
 *
 * **Example**
 * @include ex_zeroed_edges.cpp
 *
 * **Result**
 * @image html ex_zeroed_edges.png
 */
void zeroed_edges(Array      &array,
                  float       sigma = 0.25f,
                  Array      *p_noise = nullptr,
                  Vec2<float> shift = {0.f, 0.f},
                  Vec2<float> scale = {1.f, 1.f});

} // namespace hmap
