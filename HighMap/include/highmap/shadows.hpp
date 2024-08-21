/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file shadows.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Provides functions for generating shaded relief maps, shadow
 * intensity, and topographic shading.
 *
 * This header file defines functions for computing various types of shadow and
 * shading effects from heightmap data. These functions include shaded relief
 * (hillshading), shadow intensity based on grid and heightmap techniques, and
 * topographic shading.
 *
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023 Otto Link
 */

#pragma once

#include "highmap/array.hpp"
#include "macrologger.h"

namespace hmap
{

/**
 * @brief Compute the shaded relief map (hillshading) from a heightmap.
 *
 * This function calculates the shaded relief map of the input array based on
 * the sun's azimuth (direction) and zenith (elevation) angles. The shading
 * effect simulates the appearance of topographic features based on light
 * direction.
 *
 * @param z Input array representing the heightmap.
 * @param azimuth Sun azimuth (direction) in degrees.
 * @param zenith Sun zenith (elevation) in degrees.
 * @param talus_ref Reference talus used to normalize gradient computations.
 *                  It can be useful when working with true angles. Default
 * is 1.f.
 * @return Array Resulting shaded relief map.
 *
 * **Example**
 * @include ex_hillshade.cpp
 *
 * **Result**
 * @image html ex_hillshade0.png
 * @image html ex_hillshade1.png
 *
 * @see {@link topographic_shading}
 */
Array hillshade(const Array &z,
                float        azimuth,
                float        zenith,
                float        talus_ref = 1.f);

/**
 * @brief Compute the shadow intensity using a grid-based technique.
 *
 * This function calculates the shadow intensity from the input array using a
 * grid-based approach. The shadow talus parameter influences the calculation
 * of shadow intensity.
 *
 * @param z Input array representing the heightmap.
 * @param shadow_talus Parameter affecting the shadow intensity computation.
 * @return Array Resulting shadow intensity map.
 */
Array shadow_grid(const Array &z, float shadow_talus);

/**
 * @brief Compute crude shadows from a heightmap.
 *
 * This function generates crude shadow effects from the input heightmap using
 * the specified light azimuth, zenith, and distance. The method is based on
 * the technique described in https://www.shadertoy.com/view/Xlsfzl.
 *
 * @param z Input array representing the heightmap.
 * @param azimuth Light azimuth (direction) in degrees. Default is 180.f.
 * @param zenith Light zenith (elevation) in degrees. Default is 45.f.
 * @param distance Light distance. Default is 0.2f.
 * @return Array Resulting crude shadow map.
 */
Array shadow_heightmap(const Array &z,
                       float        azimuth = 180.f,
                       float        zenith = 45.f,
                       float        distance = 0.2f);

/**
 * @brief Compute the topographic shadow intensity in the range [-1, 1].
 *
 * This function calculates the topographic shadow intensity of the input array
 * based on the sun's azimuth and zenith. The shadow intensity is normalized
 * to fall within the range [-1, 1].
 *
 * @param z Input array representing the heightmap.
 * @param azimuth Sun azimuth (direction) in degrees.
 * @param zenith Sun zenith (elevation) in degrees.
 * @param talus_ref Reference talus used to normalize gradient computations.
 *                  It can be useful when working with true angles. Default
 * is 1.f.
 * @return Array Resulting topographic shadow intensity map.
 *
 * **Example**
 * @include ex_hillshade.cpp
 *
 * **Result**
 * @image html ex_hillshade0.png
 * @image html ex_hillshade1.png
 *
 * @see {@link hillshade}
 */
Array topographic_shading(const Array &z,
                          float        azimuth,
                          float        zenith,
                          float        talus_ref = 1.f);

} // namespace hmap
