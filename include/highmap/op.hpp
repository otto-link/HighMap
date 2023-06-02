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

namespace hmap
{

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
 *
 * @see {@link blend_exclusion}, {@link blend_negate}, {@link blend_overlay},
 * {@link blend_soft}
 */
Array blend_exclusion(const Array &array1, const Array &array2);

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

/**
 * @brief Clamp array values lower than a given bound with a smooth transition.
 *
 * @param array Input array.
 * @param vmin Lower bound.
 * @param k Smoothing parameter in [0, 1].
 */
void clamp_min_smooth(Array &array, float vmin, float k = 0.2f);

/**
 * @brief Clamp array values larger than a given bound.
 *
 * @param array Input array.
 * @param vmax Upper bound.
 *
 * @see {@link clamp}, {@link clamp_min}
 */
void clamp_max(Array &array, float vmax);

/**
 * @brief Clamp array values larger than a given bound with a smooth transition.
 *
 * @param array Input array.
 * @param vmax Upper bound.
 * @param k Smoothing parameter in [0, 1].
 */
void clamp_max_smooth(Array &array, float vmax, float k = 0.2f);

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
 */
Array curvature_gaussian(const Array &z);

/**
 * @brief Return the Euclidean distance transform.
 *
 * Exact transform based on Meijster et al. algorithm @cite Meijster2000.
 *
 * @param array Input array to be transformed, will be converted into binary: 1
 * wherever input equates to True, 0 elsewhere.
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
 * @brief Apply expanding, or "inflating", to emphasize the bulk of the terrain.
 *
 * @param array Input array.
 * @param ir Filter radius.
 *
 * **Example**
 * @include ex_expand.cpp
 *
 * **Result**
 * @image html ex_expand.png
 *
 * @see {@link ex_shrink}
 */
void expand(Array &array, int ir);

/**
 * @brief Linear extrapolation of values at the borders (i = 0, j = 0, ...)
 * based on inner values.
 *
 * @param array Input array.
 *
 * @see {@link fill_borders}
 *
 */
void extrapolate_borders(Array &array);

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
 * @brief Apply a gain correction of the array elements.
 *
 * Gain correction is based on a power law.
 *
 * @param array Input array.
 * @param gain Gain factor (> 0).
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * **Example**
 * @include ex_gain.cpp
 *
 * **Result**
 * @image html ex_gain.png
 */
void gain(Array &array, float gain);

/**
 * @brief Apply gamma correction to the input array.
 *
 * @param array Input array.
 * @param gamma Gamma factor (> 0).
 *
 * @warning Array values are expected to be in [0, 1].
 *
 * **Example**
 * @include ex_gamma_correction.cpp
 *
 * **Result**
 * @image html ex_gamma_correction.png
 */
void gamma_correction(Array &array, float gamma);

/**
 * @brief Return an array with buffers at the boundaries (values filled by
 * symmetry).
 *
 * @param array Input array.
 * @param buffers Buffer size {east, west, south, north}.
 * @return Array New array with buffers.
 */
Array generate_buffered_array(const Array &array, std::vector<int> buffers);

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
 * @param array Inupt array.
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
 * @brief Return the gradient in the 'x' (or 'i' index) of an array.
 *
 * @param array Inupt array.
 * @return Array Gradient.
 */
Array gradient_x(const Array &array);

/**
 * @brief Return the gradient in the 'y' (or 'j' index) of an array.
 *
 * @param array Inupt array.
 * @return Array Gradient.
 */
Array gradient_y(const Array &array);

/**
 * @brief Return the gradient talus slope of an array.
 *
 * Talus slope is locally define as the largest elevation difference between a
 * cell and its first neighbors.
 *
 * @see Thermal erosion: {@link thermal}.
 *
 * @param array Inupt array.
 * @return Array Gradient.
 */
Array gradient_talus(const Array &array);

/**
 * @brief Return the shaded relief map (or hillshading).
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
 * @brief Apply a low-pass Laplace filter.
 *
 * @param array Input array.
 * @param sigma Filtering intensity, in [0, 1].
 * @param iterations Number of iterations.
 */
void laplace(Array &array, float sigma = 0.2, int iterations = 3);

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
 * @return std::vector<float> Values.
 */
std::vector<float> linspace(float start, float stop, int num);

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
 * @return std::vector<float> Values
 */
std::vector<float> linspace_jitted(float start,
                                   float stop,
                                   int   num,
                                   float ratio,
                                   int   seed);

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
 * @brief Return the element-wise maximum of two arrays.
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @return Array Element-wise maximum array.
 */
Array maximum(const Array &array1, const Array &array2);

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
 * @brief Return the element-wise minimum of two arrays.
 *
 * @param array1 First array.
 * @param array2 Second array.
 * @return Array Element-wise minimum array.
 */
Array minimum(const Array &array1, const Array &array2);

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
 * @param gamma Gamma factor (> 0) (@see {@link gamma_correction}).
 *
 * **Example**
 * @include ex_recast.cpp
 *
 * **Result**
 * @image html ex_recast.png
 */
void recast_canyon(Array &array, const Array &vcut, float gamma = 4.f);

void recast_canyon(Array &array, float vcut, float gamma); ///< @overload

/**
 * @brief Transform heightmap to give a "peak" like appearance.
 *
 * @param array Input array.
 * @param ir Filter radius.
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
void set_borders(Array             &array,
                 std::vector<float> border_values,
                 std::vector<int>   buffer_sizes);

void set_borders(Array &array,
                 float  border_values,
                 int    buffer_sizes); ///< @overload

/**
 * @brief Apply sharpening filter (based on Laplace operator).
 *
 * @param array Input array.
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

/**
 * @brief Apply shrinking, or "deflating", to emphasize the ridges.
 *
 * @param array Input array.
 * @param ir Filter radius.
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

/**
 * @brief Return the sine of the array elements.
 *
 * @param array Input array.
 * @return Array Reference to the current object.
 */
Array sin(const Array &array);

/**
 * @brief Steepen (or flatten) the array map.
 *
 * @param array Input array.
 * @param scale Filter amplitude.
 * @param ir Filtering radius of the array gradients.
 *
 * **Example**
 * @include ex_steepen.cpp
 *
 * **Result**
 * @image html ex_steepen.png
 */
void steepen(Array &array, float scale, int ir = 8);

/**
 * @brief Steepen array values by applying a nonlinear convection operator in a
 * given direction (see to Burger's equarion for instance).
 *
 * @todo verify results.
 *
 * @param array Input array (elements expected to be in [-1, 1]).
 * @param angle Steepening direction (in degrees).
 * @param iterations Number of iterations.
 * @param dt "Time step", can be chosen smaller than 1 for fine tuning of the
 * steepening effect.
 */
void steepen_convective(Array &array,
                        float  angle,
                        int    iterations = 1,
                        float  dt = 1);

/**
 * @brief Apply filtering to the array using convolution with a cubic pulse.
 *
 * Can be used as an alternative (with a much smaller support) to Gaussian
 * smoothing. For direct comparison with Gaussian smoothing, 'ir' needs to be
 * twice larger.
 *
 * @param array Input array.
 * @param ir Pulse radius (half-width is half this radius).
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

/**
 * @brief Apply Gaussian filtering to the array.
 *
 * @param array Input array.
 * @param ir Gaussian half-width.
 *
 * **Example**
 * @include ex_smooth_gaussian.cpp
 *
 * **Result**
 * @image html ex_smooth_gaussian.png
 */
void smooth_gaussian(Array &array, int ir);

/**
 * @brief Apply cubic pulse smoothing while preserving some sharpness.
 *
 * "Cheap" version of {@link thermal_auto_bedrock}.
 *
 * @param array Input array.
 * @param ir Pulse radius.
 *
 * **Example**
 * @include ex_smooth_sharp.cpp
 *
 * **Result**
 * @image html ex_smooth_sharp.png
 *
 * @see {@link smooth_cpulse}, {@link thermal_auto_bedrock}
 */
void smooth_sharp(Array &array, int ir);

/**
 * @brief Use symmetry for to fill values at the domain borders, over a given
 * buffer depth.
 *
 * @param array Input array.
 * @param buffer_sizes Buffer size at the borders {east, west, south, north}.
 */
void sym_borders(Array &array, std::vector<int> buffer_sizes);

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
 * @todo test higher order interpolations
 *
 * @param array Input array.
 * @param dx Translation following 'i' direction, (scale is 1:1, dx = 1 => 1
 * pixel).
 * @param dy Translation following 'j' direction, (scale is 1:1, dy = 1 => 1
 * pixel).
 *
 * **Example**
 * @include ex_warp.cpp
 *
 * **Result**
 * @image html ex_warp.png
 */
void warp(Array &array, const Array &dx, const Array &dy);

/**
 * @brief Apply a warping effect to the array with displacements based on fbm
 * noise.
 *
 * @param array Input array.
 * @param scale Warping scale (scale is 1:1, scale = 1 => 1 pixel).
 * @param kw Noise wavenumbers {kx, ky} for each directions, with respect to
 * a unit domain.
 * @param seed Random seed number.
 */
void warp_fbm(Array &array, float scale, std::vector<float> kw, uint seed);

/**
 * @brief Fill values at the borders (i = 0, j = 0, ...) with zeros.
 *
 * @param array Input array.
 *
 * @see {@link fill_borders}
 */
void zeroed_borders(Array &array);

} // namespace hmap