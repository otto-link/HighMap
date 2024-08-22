/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file grid.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
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
 * @brief Expand grid by translating and copying the values of the current
 * bounding box to the 8 first neighboring bounding boxes.
 * @param x `x` coordinates.
 * @param y `y` coordinates.
 * @param value values.
 * @param bbox Bounding box.
 */
void expand_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 std::vector<float> &value,
                 Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Expand the grid by adding points on the boundaries of the bounding
 * box.
 * @param x `x` coordinates.
 * @param y `y` coordinates.
 * @param value values.
 * @param bbox Bounding box.
 * @param corner_value Value at the boundary points.
 */
void expand_grid_boundaries(std::vector<float> &x,
                            std::vector<float> &y,
                            std::vector<float> &value,
                            Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f},
                            float               boundary_value = 0.f);

/**
 * @brief Expand the grid by adding four points at the corner of the bounding
 * box.
 * @param x `x` coordinates.
 * @param y `y` coordinates.
 * @param value values.
 * @param bbox Bounding box.
 * @param corner_value Value at the four corner points.
 */
void expand_grid_corners(std::vector<float> &x,
                         std::vector<float> &y,
                         std::vector<float> &value,
                         Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f},
                         float               corner_value = 0.f);

/**
 * @brief
 * @param array Input array.
 * @param x `x` coordinates (output).
 * @param y `y` coordinates (output).
 * @param bbox Bounding box.
 * @param threshold Theshold 'background' value.
 * **Example**
 * @include ex_grid_from_array.cpp
 *
 * **Result**
 * @image html ex_grid_from_array.png
 */
void grid_from_array(Array              &array,
                     std::vector<float> &x,
                     std::vector<float> &y,
                     std::vector<float> &value,
                     Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f},
                     float               threshold = 0.f);

/**
 * @brief Return x and y coordinates of a regular grid, as two 1D vectors.
 * @param x[out] Vector x.
 * @param y[out] Vector y.
 * @param shape Shape.
 * @param bbox Bounding box.
 * @param endpoint Include or not the endpoint.
 */
void grid_xy_vector(std::vector<float> &x,
                    std::vector<float> &y,
                    Vec2<int>           shape,
                    Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f},
                    bool                endpoint = false);

/**
 * @brief Generate a random grid.
 * @param x[out] `x` coordinates (output).
 * @param y[out] `y` coordinates (output).
 * @param value[out] Random value, in [0, 1], at (x, y).
 * @param seed Random seed number.
 * @param bbox Bounding box.
 */
void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 std::vector<float> &value,
                 uint                seed,
                 Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f});

void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 uint                seed,
                 Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f}); ///< @overload

/**
 * @brief Generate a random grid.
 * @param x[out] `x` coordinates (output).
 * @param y[out] `y` coordinates (output).
 * @param value[out] Random value, in [0, 1], at (x, y).
 * @param seed Random seed number.
 * @param delta Point spacing in x and y directions.
 * @param stagger_ratio Point offset in x and y directions for every two lines
 * (to do a staggered grid), in [0, 1].
 * @param jitter Point jittering (noise) in x and y directions, in [0, 1].
 * @param bbox Bounding box.
 *
 * **Example**
 * @include ex_random_grid.cpp
 *
 * **Result**
 * @image html ex_random_grid.png
 */
void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 std::vector<float> &value,
                 uint                seed,
                 Vec2<float>         delta,
                 Vec2<float>         stagger_ratio,
                 Vec2<float>         jitter_ratio,
                 Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief
 * @param x
 * @param y
 * @param density
 * @param seed
 * @param bbox
 */
void random_grid_density(std::vector<float> &x,
                         std::vector<float> &y,
                         Array              &density,
                         uint                seed,
                         Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generate a jittered random grid.
 * @param x[out] `x` coordinates (output).
 * @param y[out] `y` coordinates (output).
 * @param scale Jittering scale, in [0, 1].
 * @param seed Random seed number.
 * @param bbox Bounding box.
 */
void random_grid_jittered(std::vector<float> &x,
                          std::vector<float> &y,
                          float               scale,
                          uint                seed,
                          Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Remove grid points that are outside a given bounding box.
 * @param x `x` coordinates.
 * @param y `y` coordinates.
 * @param value Values at points.
 * @param bbox Bounding box.
 */
void remove_grid_points_outside_bbox(std::vector<float> &x,
                                     std::vector<float> &y,
                                     std::vector<float> &value,
                                     Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Rescale coordinate (x, y) so that they fit in a unit-square box based
 * on a given initial bounding box.
 * @param x[in, out] `x` coordinates (output).
 * @param y[in, out] `y` coordinates (output).
 * @param bbox Initial bounding box.
 */
void rescale_grid_to_unit_square(std::vector<float> &x,
                                 std::vector<float> &y,
                                 Vec4<float>         bbox);

} // namespace hmap