/**
 * @file erosion.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace hmap
{

/**
 * @brief Apply thermal weathering erosion.
 *
 * @todo optimize memory usage (to avoid large constant arrays).
 *
 * Thermal erosion refers to the process in which surface sediment weakens
 * due to temperature and detaches, falling down the slopes of the terrain
 * until a resting place is reached, where smooth plateaus tend to form
 * @cite Musgrave1989.
 *
 * @param z Input array.
 * @param talus Talus limit.
 * @param bedrock Lower elevation limit.
 * @param iterations Number of iterations.
 * @param ct "Avalanching" intensity (in [0, 1]).
 *
 * **Example**
 * @include ex_thermal.cpp
 *
 * **Result**
 * @image html ex_thermal0.png
 * @image html ex_thermal1.png
 */
void thermal(Array &z,
             Array &talus,
             Array &bedrock,
             int    iterations = 50,
             float  ct = 0.4,
             bool   post_filtering = true);

void thermal(Array &array,
             Array &talus,
             int    iterations = 50,
             float  ct = 0.4,
             bool   post_filtering = true); ///< @overload

void thermal(Array &z,
             float  talus,
             int    iterations = 50,
             float  ct = 0.4,
             bool   post_filtering = true); ///< @overload

/**
 * @brief Apply thermal weathering erosion with automatic determination of the
 * bedrock.
 *
 * @todo more comprehensive documentation on auto-bedrock algo.
 * @todo fix hard-coded parameters.
 *
 * @see {@link thermal}
 *
 * @param z Input array.
 * @param talus Local talus limit.
 * @param iterations Number of iterations.
 * @param ct "Avalanching" intensity (in [0, 1]).
 *
 * **Example**
 * @include ex_thermal_auto_bedrock.cpp
 *
 * **Result**
 * @image html ex_thermal_auto_bedrock0.png
 * @image html ex_thermal_auto_bedrock1.png
 */
void thermal_auto_bedrock(Array &z,
                          Array &talus,
                          int    iterations = 50,
                          float  ct = 0.4,
                          bool   post_filtering = true);

void thermal_auto_bedrock(Array &z,
                          float  talus,
                          int    iterations = 50,
                          float  ct = 0.4,
                          bool   post_filtering = true); ///< @overload

} // namespace hmap