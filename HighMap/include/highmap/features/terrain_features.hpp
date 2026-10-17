/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file terrain_features.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file defining functions for terrain analysis and feature
 * extraction from heightmaps (connected components, geomorphons, clustering).
 *
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once

#include <array>
#include <map>
#include <vector>

#include <glm/glm.hpp>

#include "highmap/array.hpp"

/**
 * @brief Packs eight 2-bit values into a 16-bit integer.
 */
#define HMAP_PACK8(a, b, c, d, e, f, g, h)                                     \
  ((a << 15) + (b << 13) + (c << 11) + (d << 9) + (e << 7) + (f << 5) +        \
   (g << 3) + (h << 1))

namespace hmap
{

/**
 * @brief Identifies and labels connected components within a binary or labeled
 * array, with optional filtering by size.
 */
Array connected_components(
    const Array                           &array,
    float                                  surface_threshold = 0.f,
    float                                  background_value = 0.f,
    std::map<float, float>                *p_surfaces = nullptr,
    std::map<float, std::array<float, 2>> *p_centroids = nullptr);

/**
 * @brief Classifies terrain into geomorphological features based on the
 * geomorphons method.
 */
Array geomorphons(const Array &array, int irmin, int irmax, float epsilon);

/**
 * @brief Performs k-means clustering on two input arrays, grouping similar data
 * points into clusters.
 */
Array kmeans_clustering2(const Array        &array1,
                         const Array        &array2,
                         int                 nclusters,
                         std::vector<Array> *p_scoring = nullptr,
                         Array              *p_aggregate_scoring = nullptr,
                         glm::vec2           weights = {1.f, 1.f},
                         std::uint32_t       seed = 1);

/**
 * @brief Performs k-means clustering on three input arrays.
 */
Array kmeans_clustering3(const Array        &array1,
                         const Array        &array2,
                         const Array        &array3,
                         int                 nclusters,
                         std::vector<Array> *p_scoring = nullptr,
                         Array              *p_aggregate_scoring = nullptr,
                         glm::vec3           weights = {1.f, 1.f, 1.f},
                         std::uint32_t       seed = 1);

} // namespace hmap
