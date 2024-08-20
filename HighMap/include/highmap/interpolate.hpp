/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file interpolate.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "highmap/array.hpp"

namespace hmap
{

enum InterpolationMethod2D : int
{
  DELAUNAY, ///< Delaunay triangulation
  NEAREST,  ///< Nearest point
};

static std::map<InterpolationMethod2D, std::string>
    interpolation_method_2d_as_string = {{DELAUNAY, "Delaunay linear"},
                                         {NEAREST, "nearest neighbor"}};

/**
 * @brief Generic 2D interpolation.
 *
 * @param shape Output array shape.
 * @param x x coordinates of the input values.
 * @param y y coordinates of the input values.
 * @param values Input values at (x, y).
 * @param interpolation_method Interpolation method (@see interpolator2d).
 * @param p_noise_x, p_noise_y Reference to the input noise arrays.
 * @param p_stretching Local wavenumber multiplier.
 * @param bbox Domain bounding box.
 * @return Array Output array.
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

Array interpolate2d_nearest(Vec2<int>          shape,
                            std::vector<float> x,
                            std::vector<float> y,
                            std::vector<float> values,
                            Array             *p_noise_x = nullptr,
                            Array             *p_noise_y = nullptr,
                            Array             *p_stretching = nullptr,
                            Vec4<float>        bbox = {0.f, 1.f, 0.f, 1.f});

Array interpolate2d_delaunay(Vec2<int>          shape,
                             std::vector<float> x,
                             std::vector<float> y,
                             std::vector<float> values,
                             Array             *p_noise_x = nullptr,
                             Array             *p_noise_y = nullptr,
                             Array             *p_stretching = nullptr,
                             Vec4<float>        bbox = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap