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

namespace hmap
{

/**
 * @brief 2D interpolation type.
 */
enum interpolator2d : int
{
  delaunay, ///< Delaunay triangular
  bilinear, ///< Bilinear
  thinplate ///< Thinplate
};

Array interpolate2d(Vec2<int>          shape,
                    std::vector<float> x,
                    std::vector<float> y,
                    std::vector<float> values,
                    int                interpolation_method,
                    Array             *p_noise_x = nullptr,
                    Array             *p_noise_y = nullptr,
                    Vec2<float>        shift = {0.f, 0.f},
                    Vec2<float>        scale = {1.f, 1.f});

} // namespace hmap