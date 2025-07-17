/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file shortest_path.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Implements shortest path algorithms including Dijkstra's method for 2D
 * array data representation.
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Finds the path with the lowest elevation and elevation difference
 * between two points in a 2D array using Dijkstra's algorithm.
 *
 * This function calculates the shortest path considering both elevation and
 * elevation differences. It uses a cost function that balances between absolute
 * elevation and elevation change. The path is determined by minimizing the
 * combined cost function.
 *
 * @see                       @cite Dijkstra1971 and
 *                            https://math.stackexchange.com/questions/3088292
 *
 * @warning The `elevation_ratio` parameter must be less than 1 for the
 * algorithm to converge properly.
 *
 * @param ij_start            Starting coordinates (i, j) for the pathfinding.
 * @param ij_end              Ending coordinates (i, j) for the pathfinding.
 * @param i_path[out]         Vector to store the resulting shortest path
 *                            indices in the i direction.
 * @param j_path[out]         Vector to store the resulting shortest path
 *                            indices in the j direction.
 * @param elevation_ratio     Balance factor between absolute elevation and
 *                            elevation difference in the cost function. Should
 *                            be in the range [0, 1[.
 * @param distance_exponent   Exponent used in the distance calculation between
 *                            points. A higher exponent increases the cost of
 *                            elevation gaps, encouraging the path to minimize
 *                            elevation changes and reduce overall cumulative
 *                            elevation gain.
 * @param upward_penalization Penalize upstream slopes.
 * @param p_mask_nogo         Optional pointer to an array mask that defines
 *                            areas to avoid during pathfinding.
 *
 * **Example**
 * @include ex_find_path_dijkstra.cpp
 *
 * **Result**
 * @image html ex_find_path_dijkstra0.png
 * @image html ex_find_path_dijkstra1.png
 * @image html ex_find_path_dijkstra2.png
 */
void find_path_dijkstra(const Array      &z,
                        Vec2<int>         ij_start,
                        Vec2<int>         ij_end,
                        std::vector<int> &i_path,
                        std::vector<int> &j_path,
                        float             elevation_ratio = 0.1f,
                        float             distance_exponent = 2.f,
                        float             upward_penalization = 1.f,
                        const Array      *p_mask_nogo = nullptr);

void find_path_dijkstra(const Array                   &z,
                        Vec2<int>                      ij_start,
                        std::vector<Vec2<int>>         ij_end_list,
                        std::vector<std::vector<int>> &i_path_list,
                        std::vector<std::vector<int>> &j_path_list,
                        float                          elevation_ratio = 0.1f,
                        float                          distance_exponent = 2.f,
                        float        upward_penalization = 1.f,
                        const Array *p_mask_nogo = nullptr);

} // namespace hmap