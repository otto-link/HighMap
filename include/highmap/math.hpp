/**
 * @file math.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace hmap
{

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
 * @image html ex_convolve1d_ij0.png
 * @image html ex_convolve1d_ij1.png
 *
 * @see {@link convolve1d_j}
 */
Array convolve1d_i(Array &array, const std::vector<float> &kernel);

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
 * @image html ex_convolve1d_ij0.png
 * @image html ex_convolve1d_ij1.png
 *
 * @see {@link convolve1d_i}
 */
Array convolve1d_j(Array &array, const std::vector<float> &kernel);

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
Array convolve2d(Array &array, Array &kernel);

/**
 * @brief Return the convolution product of the array with a given kernel. The
 * output has a smaller size than the input.
 *
 * @param array Input array.
 * @param kernel Kernel array.
 * @return Array Convolution result (shape: {array.shape[0] - kernel.shape[0],
 * array.shape[1] - kernel.shape[1]}).
 */
Array convolve2d_truncated(Array &array, Array &kernel);

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
 * @image html ex_convolve2d_svd0.png
 * @image html ex_convolve2d_svd1.png
 * @image html ex_convolve2d_svd2.png
 */
Array convolve2d_svd(Array &z, Array &kernel, int rank = 3);

/**
 * @brief Return the polar angle of the gradient of an array.
 *
 * @param array Input array.
 * @param downward If set set true, return the polar angle of the downward
 * slope.
 * @return Array Gradient angle, in radians, in [-\pi, \pi].
 */
Array gradient_angle(Array &array, bool downward = false);

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
Array gradient_norm(Array &array);

/**
 * @brief Return the gradient in the 'x' (or 'i' index) of an array.
 *
 * @param array Inupt array.
 * @return Array Gradient.
 */
Array gradient_x(Array &array);

/**
 * @brief Return the gradient in the 'y' (or 'j' index) of an array.
 *
 * @param array Inupt array.
 * @return Array Gradient.
 */
Array gradient_y(Array &array);

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
Array gradient_talus(Array &array);

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
Array hypot(Array &array1, Array &array2);

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
 * @brief Return the array elements raised to the power 'exp'.
 *
 * @param exp Exponent.
 * @return Array Reference to the current object.
 */
Array pow(Array &array, float exp);

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

} // namespace hmap