/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file interpolate1d.hpp
 * @author
 * Otto Link (otto.link.bv@gmail.com)
 * @brief Defines a 1D interpolation class using the GSL (GNU Scientific
 * Library).
 *
 * This file contains the `Interpolator1D` class and `InterpolationMethod1D`
 * enum, which provide a simple interface for performing 1D interpolation with
 * various methods available in the GSL. The interpolation methods include
 * Akima, cubic, linear, polynomial, and Steffen interpolation.
 *
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023 Otto Link
 * Distributed under the terms of the GNU General Public License.
 * The full license is in the file LICENSE, distributed with this software.
 */
#pragma once
#include <map>
#include <vector>

#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

namespace hmap
{

/**
 * @enum InterpolationMethod1D
 * @brief Enumeration of the available 1D interpolation methods.
 *
 * This enumeration defines the different types of interpolation methods that
 * can be used with the `Interpolator1D` class.
 */
enum InterpolationMethod1D : int
{
  AKIMA,          ///< Akima interpolation
  AKIMA_PERIODIC, ///< Akima periodic interpolation
  CUBIC,          ///< Cubic spline interpolation
  CUBIC_PERIODIC, ///< Cubic spline periodic interpolation
  LINEAR,         ///< Linear interpolation
  POLYNOMIAL,     ///< Polynomial interpolation
  STEFFEN,        ///< Steffen interpolation (monotonic)
};

/**
 * @class Interpolator1D
 * @brief A class for performing 1D interpolation using the GSL library.
 *
 * The `Interpolator1D` class provides an interface for performing 1D
 * interpolation on a set of data points. The interpolation methods supported
 * are defined by the `InterpolationMethod1D` enumeration. The class uses the
 * GSL library for the interpolation computations.
 *
 *
 * **Example**
 * @include ex_interpolate1d.cpp
 *
 * **Result**
 * @image html ex_interpolate1d_0.png
 * @image html ex_interpolate1d_1.png
 * @image html ex_interpolate1d_2.png
 */
class Interpolator1D
{
public:
  /**
   * @brief Constructs an `Interpolator1D` object with the given data and
   * method.
   *
   * This constructor initializes the interpolation object with the provided
   * x and y data points and the specified interpolation method.
   *
   * @param x A vector of x coordinates (independent variable).
   * @param y A vector of y coordinates (dependent variable).
   * @param method The interpolation method to use (default is linear).
   *
   * @throws std::invalid_argument if x and y have different sizes or if there
   * are fewer than two points. An exception is also thrown if the method
   *         requires monotonic data and the provided data is not monotonic.
   */
  Interpolator1D(const std::vector<float> &x,
                 const std::vector<float> &y,
                 InterpolationMethod1D method = InterpolationMethod1D::LINEAR);

  /**
   * @brief Destructor for `Interpolator1D`.
   *
   * The destructor frees the GSL resources used by the interpolation object.
   */
  ~Interpolator1D();

  /**
   * @brief Function call operator for performing interpolation.
   *
   * This operator overload allows the `Interpolator1D` object to be called as a
   * function, directly returning the interpolated value at the specified x
   * coordinate. It serves as a shorthand for the `interpolate()` method.
   *
   * @param x The x coordinate at which to interpolate.
   * @return The interpolated y value corresponding to the given x.
   */
  float operator()(float x) const;

  /**
   * @brief Interpolates the value at the specified x coordinate.
   *
   * This function computes the interpolated y value corresponding to the given
   * x coordinate using the previously specified interpolation method.
   *
   * @param x The x coordinate at which to interpolate.
   * @return The interpolated y value.
   */
  float interpolate(float x) const;

private:
  gsl_spline         *interp; ///< GSL spline object used for interpolation
  gsl_interp_accel   *accel_; ///< GSL interpolation accelerator
  std::vector<double> x_data; ///< x data converted to double for GSL
  std::vector<double> y_data; ///< y data converted to double for GSL
  double              xmin;   ///< Minimum x value in the data set
  double              xmax;   ///< Maximum x value in the data set
};

} // namespace hmap
