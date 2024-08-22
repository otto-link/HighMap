/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file interpolate2d.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for 2D interpolation methods.
 *
 * This file provides declarations for functions and enumerations related to
 * 2D interpolation. It supports different interpolation methods, including
 * Delaunay triangulation and nearest point interpolation. These functions
 * allow for the interpolation of values on a 2D grid using various techniques.
 *
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023 Otto Link
 *
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
 * @brief Generic 2D interpolation function.
 *
 * This function performs interpolation on a 2D grid using the specified
 * interpolation method. It can optionally apply noise and stretching to
 * the input data before interpolation.
 *
 * @param shape Output array shape.
 * @param x x coordinates of the input values.
 * @param y y coordinates of the input values.
 * @param values Input values at (x, y).
 * @param interpolation_method Interpolation method (see @ref
 * InterpolationMethod2D).
 * @param p_noise_x Pointer to the input noise array in the x direction
 * (optional).
 * @param p_noise_y Pointer to the input noise array in the y direction
 * (optional).
 * @param p_stretching Pointer to the local wavenumber multiplier array
 * (optional).
 * @param bbox Domain bounding box (default: {0.f, 1.f, 0.f, 1.f}).
 * @return Array Output array with interpolated values.
 */
Array interpolate2d(Vec2<int>             shape,
                    std::vector<float>    x,
                    std::vector<float>    y,
                    std::vector<float>    values,
                    InterpolationMethod2D interpolation_method,
                    Array                *p_noise_x = nullptr,
                    Array                *p_noise_y = nullptr,
                    Array                *p_stretching = nullptr,
                    Vec4<float>           bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief 2D interpolation using the nearest neighbor method.
 *
 * This function performs 2D interpolation by assigning the value of the
 * nearest point to each point in the output grid.
 *
 * @param shape Output array shape.
 * @param x x coordinates of the input values.
 * @param y y coordinates of the input values.
 * @param values Input values at (x, y).
 * @param p_noise_x Pointer to the input noise array in the x direction
 * (optional).
 * @param p_noise_y Pointer to the input noise array in the y direction
 * (optional).
 * @param p_stretching Pointer to the local wavenumber multiplier array
 * (optional).
 * @param bbox Domain bounding box (default: {0.f, 1.f, 0.f, 1.f}).
 * @return Array Output array with interpolated values.
 */
Array interpolate2d_nearest(Vec2<int>          shape,
                            std::vector<float> x,
                            std::vector<float> y,
                            std::vector<float> values,
                            Array             *p_noise_x = nullptr,
                            Array             *p_noise_y = nullptr,
                            Array             *p_stretching = nullptr,
                            Vec4<float>        bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief 2D interpolation using the Delaunay triangulation method.
 *
 * This function performs 2D interpolation by generating a Delaunay
 * triangulation of the input points and interpolating the values within each
 * triangle.
 *
 * @param shape Output array shape.
 * @param x x coordinates of the input values.
 * @param y y coordinates of the input values.
 * @param values Input values at (x, y).
 * @param p_noise_x Pointer to the input noise array in the x direction
 * (optional).
 * @param p_noise_y Pointer to the input noise array in the y direction
 * (optional).
 * @param p_stretching Pointer to the local wavenumber multiplier array
 * (optional).
 * @param bbox Domain bounding box (default: {0.f, 1.f, 0.f, 1.f}).
 * @return Array Output array with interpolated values.
 */
Array interpolate2d_delaunay(Vec2<int>          shape,
                             std::vector<float> x,
                             std::vector<float> y,
                             std::vector<float> values,
                             Array             *p_noise_x = nullptr,
                             Array             *p_noise_y = nullptr,
                             Array             *p_stretching = nullptr,
                             Vec4<float>        bbox = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap
