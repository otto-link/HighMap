/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file math.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Provides core mathematical utilities for procedural generation,
 * including trigonometry, exponential/logarithmic functions, smooth transitions
 * (smoothstep), distance metrics (Chebyshev, Euclidian), and phasor profiles.
 * Offers optimized approximations (hypotenuse, inverse sqrt) and array-based
 * operations for terrain modeling and noise synthesis.
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.707106781186547524401
#endif

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Distance function type.
 */
enum DistanceFunction : int
{
  CHEBYSHEV, ///< Chebyshev
  EUCLIDIAN, ///< Euclidian
  EUCLISHEV, ///< Euclidian and Chebyshev mix
  MANHATTAN, ///< Manhattan
};

/**
 * @brief Phasor angular profile type.
 */
enum PhasorProfile : int
{
  COSINE_BULKY,
  COSINE_PEAKY,
  COSINE_SQUARE,
  COSINE_STD,
  TRIANGLE,
};

/**
 * @brief Return the absolute value of the array elements.
 *
 * @param  array Input array.
 * @return       Array Output array.
 */
Array abs(const Array &array);

/**
 * @brief Return the smooth absolute value of the array elements.
 *
 * @param  array  Input array.
 * @param  k      Smoothing coefficient.
 * @param  vshift Reference value for the "zero" value of the absolute value
 *                (default is zero).
 * @return        Array Output array.
 *
 * **Example**
 * @include ex_abs_smooth.cpp
 *
 * **Result**
 * @image html ex_abs_smooth.png
 */
Array abs_smooth(const Array &array, float mu, const Array &vshift);
Array abs_smooth(const Array &array, float mu, float vshift); ///< @overload
Array abs_smooth(const Array &array, float mu);               ///< @overload
float abs_smooth(float a, float mu);                          ///< @overload

/**
 * @brief Return the almost unit identity function.
 *
 * Function that maps the unit interval to itself with zero derivative at 0 and
 * "one" derivative at 1 (see <a
 * href=https://iquilezles.org/articles/functions/>Inigo Quilez's articles</a>).
 *
 * @param  array Input array.
 * @return       Array Output array.
 */
Array almost_unit_identity(const Array &array);
float almost_unit_identity(float x); ///< @overload

/**
 * @brief Return the almost unit identity function (with a second-order
 * derivative equals 0 at x = 1 also to avoid discontinuities in some cases)
 *
 * @param  x Input.
 * @return   float Output.
 */
float almost_unit_identity_c2(float x);

/**
 * @brief Return the approximate hypothenuse of two numbers.
 *
 * @param  a a
 * @param  b a
 * @return   float ~sqrt(a**2 + b**2)
 */
inline float approx_hypot(float a, float b)
{
  a = std::abs(a);
  b = std::abs(b);
  if (a > b) std::swap(a, b);
  return 0.414f * a + b;
}

/**
 * @brief Return the approximate inverse square root of a number.
 *
 * @param  a a
 * @return   float ~1/sqrt(a)
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
 * @param  array Input array.
 * @return       Array Reference to the current object.
 */
Array atan(const Array &array);

/**
 * @brief Computes the element-wise arctangent of two arrays, considering the
 * signs of both inputs.
 *
 * This function calculates the arctangent of the ratio `y/x` for each
 * corresponding element in the input arrays, while taking into account the
 * quadrant of the angle. The output values are in radians and range from `-π`
 * to `π`.
 *
 * @param  y A 2D array representing the numerator values.
 * @param  x A 2D array representing the denominator values.
 * @return   A 2D array where each element is the result of `atan2(y(i, j), x(i,
 *           j))`.
 */
Array atan2(const Array &y, const Array &x);

/**
 * @brief Return the cosine of the array elements.
 *
 * @param  array Input array.
 * @return       Array Reference to the current object.
 */
Array cos(const Array &array);

/**
 * @brief Return the exponantial of the array elements.
 *
 * @param  array Input array.
 * @return       Array Reference to the current object.
 */
Array exp(const Array &array);

float gain(float x, float factor);

/**
 * @brief Return the Gaussian of the array elements.
 *
 * @param  array Input array.
 * @param  sigma Gaussian half-width.
 * @return       Array Reference to the current object.
 *
 * **Example**
 * @include ex_gaussian_decay.cpp
 *
 * **Result**
 * @image html ex_gaussian_decay.png
 */
Array gaussian_decay(const Array &array, float sigma);

/**
 * @brief Return the requested distance function.
 * @param  dist_fct Distance function type.
 * @return          Distance function.
 */
std::function<float(float, float)> get_distance_function(
    DistanceFunction dist_fct);

/**
 * @brief Generates a function representing a phasor profile based on the
 * specified type and parameters.
 *
 * This function returns a callable object (`std::function`) that computes the
 * value of the specified phasor profile for a given phase angle (phi).
 * Optionally, it can compute the average value of the profile over the range
 * [-π, π].
 *
 * @param  phasor_profile The type of phasor profile to generate.
 * @param  delta          A parameter that can influence the profile (depending
 *                        on the profile choice).
 * @param  p_profile_avg  Optional pointer to a float. If not `nullptr`, it will
 *                        store the average value of the profile over the range
 *                        [-π, π].
 * @return                A `std::function<float(float)>` that computes the
 *                        phasor profile for a given phase angle.
 *
 * @throws std::invalid_argumentIftheprovided`phasor_profile`isinvalid.
 *
 * @note The average value is computed using numerical integration over 50
 * sample points within [-π, π].
 */
std::function<float(float)> get_phasor_profile_function(
    const PhasorProfile &phasor_profile,
    float                delta,
    float               *p_profile_avg = nullptr);

/**
 * @brief Computes the highest power of 2 less than or equal to the given
 * number.
 *
 * @param  n The input integer for which the highest power of 2 is to be
 *           determined.
 * @return   The highest power of 2 less than or equal to `n`.
 */
int highest_power_of_2(int n);

/**
 * @brief Return the square root of the sum of the squares of the two input
 * arrays.
 *
 * @relates Map
 *
 * @param  array1 First array.
 * @param  array2 Second array.
 * @return        Array Hypothenuse.
 */
Array hypot(const Array &array1, const Array &array2);

/**
 * @brief Return the linear interpolation between two arrays by a parameter t.
 *
 * @param  array1 First array.
 * @param  array2 Second array.
 * @param  t      Interpolation parameter (in [0, 1]).
 * @return        Array Interpolated array.
 */
Array lerp(const Array &array1, const Array &array2, const Array &t);
Array lerp(const Array &array1, const Array &array2, float t); ///< @overload
float lerp(float a, float b, float t);                         ///< @overload

/**
 * @brief Return the log10 of the array elements.
 *
 * @param  array Input array.
 * @return       Array Reference to the current object.
 */
Array log10(const Array &array);

/**
 * @brief Return the array elements raised to the power 'exp'.
 *
 * @param  exp Exponent.
 * @return     Array Reference to the current object.
 */
Array pow(const Array &array, float exp);

/**
 * @brief Interpret the input array `dr` as a radial displacement and convert it
 * to a pair of displacements `dx` and `dy` in cartesian coordinates.
 * @param dr        Radial displacement.
 * @param dx        Displacent in x direction (output).
 * @param dy        Displacent in y direction (output).
 * @param smoothing Smoothing parameter to avoid discontinuity at the origin.
 * @param center    Origin center.
 * @param bbox      Domain bounding box.
 *
 * **Example**
 * @include ex_radial_displacement_to_xy.cpp
 *
 * **Result**
 * @image html ex_radial_displacement_to_xy.png
 * */
void radial_displacement_to_xy(const Array &dr,
                               Array       &dx,
                               Array       &dy,
                               float        smoothing = 1.f,
                               Vec2<float>  center = {0.5f, 0.5f},
                               Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Return the sine of the array elements.
 *
 * @param  array Input array.
 * @return       Array Reference to the current object.
 */
Array sin(const Array &array);

/**
 * @brief Return the 3rd order smoothstep function of the array elements.
 *
 * @param  array Input array.
 * @param  vmin  Lower bound.
 * @param  vmax  Upper bound.
 * @return       Array Output array.
 *
 * **Example**
 * @include ex_smoothstep.cpp
 *
 * **Result**
 * @image html ex_smoothstep.png
 */
Array smoothstep3(const Array &array, float vmin = 0.f, float vmax = 1.f);

/**
 * @brief Return the 3rd order smoothstep function.
 *
 * @param  x Input.
 * @return   float Output.
 */
float smoothstep3(float x);

/**
 * @brief Return the 3rd order smoothstep function, with zero derivative only at
 * 0.
 *
 * @param  x Input.
 * @return   float Output.
 */
float smoothstep3_lower(float x);
Array smoothstep3_lower(const Array &x); ///< @overload

/**
 * @brief Return the 3rd order smoothstep function, with zero derivative only at
 * 1.
 *
 * @param  x Input.
 * @return   float Output.
 */
float smoothstep3_upper(float x);
Array smoothstep3_upper(const Array &x); ///< @overload

/**
 * @brief Return the 5rd order smoothstep function of the array elements.
 *
 * @param  array Input array.
 * @param  vmin  Lower bound.
 * @param  vmax  Upper bound.
 * @return       Array Output array.
 *
 * **Example**
 * @include ex_smoothstep.cpp
 *
 * **Result**
 * @image html ex_smoothstep.png
 */
Array smoothstep5(const Array &array, float vmin = 0.f, float vmax = 1.f);
Array smoothstep5(const Array &array,
                  const Array &vmin,
                  const Array &vmax); ///< @overload

/**
 * @brief Return the 5rd order smoothstep function.
 *
 * @param  x Input.
 * @return   float Output.
 */
float smoothstep5(float x);

/**
 * @brief Return the 5rd order smoothstep function, with zero derivative only at
 * 0.
 *
 * @param  x Input.
 * @return   float Output.
 */
float smoothstep5_lower(float x);
Array smoothstep5_lower(const Array &x); ///< @overload

/**
 * @brief Return the 5rd order smoothstep function, with zero derivative only at
 * 1.
 *
 * @param  x Input.
 * @return   float Output.
 */
float smoothstep5_upper(float x);
Array smoothstep5_upper(const Array &x); ///< @overload

/**
 * @brief Return the 7th order smoothstep function.
 *
 * @param  x Input.
 * @return   float Output.
 */
float smoothstep7(float x);
Array smoothstep7(const Array &x); ///< @overload

/**
 * @brief Return the square root of the array elements.
 *
 * @param  array Input array.
 * @return       Array Reference to the current object.
 */
Array sqrt(const Array &array);

} // namespace hmap
