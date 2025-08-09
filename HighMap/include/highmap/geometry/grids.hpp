/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file grid.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1 Initial version.
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023 Otto Link
 *
 * This software is distributed under the terms of the GNU General Public
 * License. The full license is available in the LICENSE file distributed with
 * this software.
 */

#pragma once
#include <vector>

#include "highmap/algebra.hpp"
#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Converts a length value to a pixel index in a discretized space.
 *
 * This function maps a floating-point length `x` to an integer pixel index,
 * considering the total number of pixels `nx` and a scaling factor `scale`.
 * Optionally, it can enforce lower and upper limits on the output index.
 *
 * @param  x       The length value to be converted.
 * @param  nx      The number of pixels in the discretized space.
 * @param  lim_inf If nonzero, enforces a minimum index of 1.
 * @param  lim_sup If nonzero, enforces a maximum index of `nx - 1`.
 * @param  scale   The scaling factor relating the length to the pixel space.
 * @return         The computed pixel index.
 */
int convert_length_to_pixel(float x,
                            int   nx,
                            bool  lim_inf = true,
                            bool  lim_sup = false,
                            float scale = 1.f);

/**
 * @brief Return x and y coordinates of a regular grid, as two 1D vectors.
 * @param x[out]   Vector x.
 * @param y[out]   Vector y.
 * @param shape    Shape.
 * @param bbox     Bounding box.
 * @param endpoint Include or not the endpoint.
 */
void grid_xy_vector(std::vector<float> &x,
                    std::vector<float> &y,
                    Vec2<int>           shape,
                    Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f},
                    bool                endpoint = false);

} // namespace hmap
