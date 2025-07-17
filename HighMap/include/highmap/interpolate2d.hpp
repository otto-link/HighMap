/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file interpolate2d.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for 2D interpolation methods.
 *
 * This file provides declarations for functions and enumerations related to 2D
 * interpolation. It supports different interpolation methods, including
 * Delaunay triangulation and nearest point interpolation. These functions allow
 * for the interpolation of values on a 2D grid using various techniques.
 *
 * @copyright Copyright (c) 2023 Otto Link
 */

#pragma once
#include <map>

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @enum InterpolationMethod2D
 * @brief Enumeration of 2D interpolation methods.
 *
 * This enum defines the available methods for 2D interpolation, such as
 * Delaunay triangulation and nearest point interpolation.
 */
enum InterpolationMethod2D : int
{
  DELAUNAY, ///< Delaunay triangulation method for 2D interpolation.
  NEAREST,  ///< Nearest point method for 2D interpolation.
};

/**
 * @brief A map associating 2D interpolation methods with their string
 * representations.
 *
 * This static map provides a human-readable string for each interpolation
 * method defined in the @ref InterpolationMethod2D enum.
 */
static std::map<InterpolationMethod2D, std::string>
    interpolation_method_2d_as_string = {{DELAUNAY, "Delaunay linear"},
                                         {NEAREST, "nearest neighbor"}};

/**
 * @brief Compute the bilinear interpolated value from four input values.
 *
 * This function calculates the interpolated value at a point within a grid
 * using bilinear interpolation based on four surrounding values.
 *
 * @param  f00 Value at (u, v) = (0, 0).
 * @param  f10 Value at (u, v) = (1, 0).
 * @param  f01 Value at (u, v) = (0, 1).
 * @param  f11 Value at (u, v) = (1, 1).
 * @param  u   The interpolation parameter in the x-direction, expected in [0,
 *             1).
 * @param  v   The interpolation parameter in the y-direction, expected in [0,
 *             1).
 * @return     float The bilinear interpolated value.
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

inline float cubic_interpolate(float p[4], float x)
{
  return p[1] + 0.5 * x *
                    (p[2] - p[0] +
                     x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] +
                          x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}

/**
 * @brief Generic 2D interpolation function.
 *
 * This function performs interpolation on a 2D grid using the specified
 * interpolation method. It can optionally apply noise and stretching to the
 * input data before interpolation.
 *
 * @param  shape                Output array shape.
 * @param  x                    x coordinates of the input values.
 * @param  y                    y coordinates of the input values.
 * @param  values               Input values at (x, y).
 * @param  interpolation_method Interpolation method (see @ref
 *                              InterpolationMethod2D).
 * @param  p_noise_x            Pointer to the input noise array in the x
 *                              direction (optional).
 * @param  p_noise_y            Pointer to the input noise array in the y
 *                              direction (optional).
 * @param  p_stretching         Pointer to the local wavenumber multiplier array
 *                              (optional).
 * @param  bbox                 Domain bounding box (default: {0.f, 1.f, 0.f,
 *                              1.f}).
 * @return                      Array Output array with interpolated values.
 */
Array interpolate2d(Vec2<int>                 shape,
                    const std::vector<float> &x,
                    const std::vector<float> &y,
                    const std::vector<float> &values,
                    InterpolationMethod2D     interpolation_method,
                    const Array              *p_noise_x = nullptr,
                    const Array              *p_noise_y = nullptr,
                    const Array              *p_stretching = nullptr,
                    Vec4<float>               bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief 2D interpolation using the nearest neighbor method.
 *
 * This function performs 2D interpolation by assigning the value of the nearest
 * point to each point in the output grid.
 *
 * @param  shape        Output array shape.
 * @param  x            x coordinates of the input values.
 * @param  y            y coordinates of the input values.
 * @param  values       Input values at (x, y).
 * @param  p_noise_x    Pointer to the input noise array in the x direction
 *                      (optional).
 * @param  p_noise_y    Pointer to the input noise array in the y direction
 *                      (optional).
 * @param  p_stretching Pointer to the local wavenumber multiplier array
 *                      (optional).
 * @param  bbox         Domain bounding box (default: {0.f, 1.f, 0.f, 1.f}).
 * @return              Array Output array with interpolated values.
 */
Array interpolate2d_nearest(Vec2<int>                 shape,
                            const std::vector<float> &x,
                            const std::vector<float> &y,
                            const std::vector<float> &values,
                            const Array              *p_noise_x = nullptr,
                            const Array              *p_noise_y = nullptr,
                            const Array              *p_stretching = nullptr,
                            Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief 2D interpolation using the Delaunay triangulation method.
 *
 * This function performs 2D interpolation by generating a Delaunay
 * triangulation of the input points and interpolating the values within each
 * triangle.
 *
 * @param  shape        Output array shape.
 * @param  x            x coordinates of the input values.
 * @param  y            y coordinates of the input values.
 * @param  values       Input values at (x, y).
 * @param  p_noise_x    Pointer to the input noise array in the x direction
 *                      (optional).
 * @param  p_noise_y    Pointer to the input noise array in the y direction
 *                      (optional).
 * @param  p_stretching Pointer to the local wavenumber multiplier array
 *                      (optional).
 * @param  bbox         Domain bounding box (default: {0.f, 1.f, 0.f, 1.f}).
 * @return              Array Output array with interpolated values.
 */
Array interpolate2d_delaunay(Vec2<int>                 shape,
                             const std::vector<float> &x,
                             const std::vector<float> &y,
                             const std::vector<float> &values,
                             const Array              *p_noise_x = nullptr,
                             const Array              *p_noise_y = nullptr,
                             const Array              *p_stretching = nullptr,
                             Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap
