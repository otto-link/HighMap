/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file filters.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for filter-related functions and classes.
 *
 * This header file contains declarations for various filter functions and
 * classes used within the project. These filters are typically used for
 * processing data streams or collections according to specific criteria.
 *
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once

#include "highmap/array.hpp"

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
 * @brief Apply histogram adjustement to the array values.
 *
 * @param array Input array.
 */
void equalize(Array &array);
void equalize(Array &array, Array *p_mask); ///< @overload

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
 * @brief Modifies a terrain array by filling it with talus slopes.
 *
 * This function applies a talus formation algorithm to an existing terrain
 * array, adjusting the heights to create natural-looking slopes. The process
 * involves random perturbations influenced by noise to simulate erosion or
 * sediment transport.
 *
 * @param z A reference to the 2D array representing the terrain heights. The
 * function modifies this array in place to introduce talus slopes.
 * @param talus The critical slope angle that determines where material will
 * move from higher elevations to lower ones. Slopes steeper than this value
 *              will be flattened by material transport.
 * @param seed The seed for the random number generator, ensuring
 * reproducibility of the noise effects in the talus formation process. The same
 * seed will produce the same terrain modifications.
 * @param noise_ratio A parameter that controls the amount of randomness or
 * noise introduced in the talus formation process. The default value is 0.2.
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

} // namespace hmap
