/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file hydrology.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for hydrological modeling functions and utilities.
 *
 * This header file declares functions and utilities for hydrological modeling,
 * including tools for computing flow directions, flow accumulation, and
 * identifying flow sinks within heightmaps. It supports multiple flow direction
 * models and the D8 model for flow direction and accumulation calculations.
 *
 * Key functionalities include:
 * - Computation of flow directions and flow accumulations using various models.
 * - Identification of flow sinks in heightmaps.
 * - Support for multiple flow direction models with customizable flow-partition
 * exponents.
 *
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Computes the number of drainage paths for each cell based on the D8
 * flow direction model.
 *
 * This function calculates the number of neighboring cells that have flow
 * directions pointing to the current cell according to the D8 model. The result
 * is an array where each cell contains the count of its incoming flow
 * directions.
 *
 * @param d8 Input array representing the flow directions according to the D8
 * model. Each cell value indicates the direction of flow according to the D8
 * convention.
 * @return Array An array where each cell contains the number of incoming flow
 * directions pointing to it.
 */
Array d8_compute_ndip(const Array &d8);

/**
 * @brief Identifies the indices of flow sinks within the heightmap.
 *
 * This function locates the cells in the heightmap that are flow sinks (cells
 * with no outflow) and returns their indices. Flow sinks are cells that do not
 * direct flow to any other cell.
 *
 * @param z Input array representing the heightmap values.
 * @param is Output vector containing the row indices `i` of the flow sinks.
 * @param js Output vector containing the column indices `j` of the flow sinks.
 *
 * **Example**
 * @include ex_find_flow_sinks.cpp
 */
void find_flow_sinks(const Array      &z,
                     std::vector<int> &is,
                     std::vector<int> &js);

/**
 * @brief Computes the flow accumulation for each cell using the D8 flow
 * direction model.
 *
 * This function calculates the flow accumulation for each cell in the heightmap
 * based on the D8 flow direction model \cite Zhou2019. Flow accumulation
 * represents the total amount of flow that accumulates at each cell from
 * upstream cells.
 *
 * @param z Input array representing the heightmap values.
 * @return Array An array where each cell contains the computed flow
 * accumulation.
 *
 * **Example**
 * @include ex_flow_accumulation_d8.cpp
 *
 * **Result**
 * @image html ex_flow_accumulation_d80.png
 * @image html ex_flow_accumulation_d81.png
 *
 * @see flow_direction_d8
 */
Array flow_accumulation_d8(const Array &z);
/**
 * @brief Computes the flow accumulation for each cell using the Multiple Flow
 * Direction (MFD) model.
 *
 * This function calculates the flow accumulation for each cell based on the MFD
 * model \cite Qin2007. Flow accumulation represents the total amount of flow
 * that accumulates at each cell from upstream cells. The flow-partition
 * exponent is locally defined using a reference talus value, where smaller
 * values of `talus_ref` will lead to narrower flow streams. The maximum talus
 * value of the heightmap can be used as a reference.
 *
 * @param z Input array representing the heightmap values.
 * @param talus_ref Reference talus used to locally define the flow-partition
 * exponent. Small values will result in thinner flow streams.
 * @return Array An array where each cell contains the computed flow
 * accumulation.
 *
 * **Example**
 * @include ex_flow_accumulation_dinf.cpp
 *
 * **Result**
 * @image html ex_flow_accumulation_dinf0.png
 * @image html ex_flow_accumulation_dinf1.png
 *
 * @see flow_direction_dinf, flow_accumulation_d8
 */
Array flow_accumulation_dinf(const Array &z, float talus_ref);

/**
 * @brief Computes the flow direction from each cell to its downslope neighbor
 * using the D8 model.
 *
 * This function calculates the direction of flow for each cell in the heightmap
 * using the D8 flow direction model @cite Greenlee1987. The D8 model defines
 * eight possible flow directions as follows:
 * @verbatim
 *   5 6 7
 *   4 . 0
 *   3 2 1
 * @endverbatim
 *
 * @param z Input array representing the heightmap values.
 * @return Array An array where each cell contains the flow direction according
 * to the D8 nomenclature.
 *
 * @see flow_accumulation_d8
 */
Array flow_direction_d8(const Array &z);

/**
 * @brief Computes the flow direction and weights for each direction using the
 * Multiple Flow Direction (MFD) model.
 *
 * This function calculates the flow direction for each cell and provides the
 * weight for each possible flow direction using the MFD model \cite Qin2007.
 * The flow-partition exponent is determined using a reference talus value.
 * Smaller values of `talus_ref` will lead to thinner flow streams. The maximum
 * talus value of the heightmap can be used as a reference.
 *
 * @param z Input array representing the heightmap values.
 * @param talus_ref Reference talus used to locally define the flow-partition
 * exponent. Smaller values will result in thinner flow streams.
 * @return std::vector<Array> A vector of arrays, each containing the weights
 * for flow directions at every cell.
 */
std::vector<Array> flow_direction_dinf(const Array &z, float talus_ref);

} // namespace hmap