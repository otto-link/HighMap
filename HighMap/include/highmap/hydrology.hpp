/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

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
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once

#include "highmap/array.hpp"
#include "highmap/geometry/path.hpp"

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
 * @param  d8 Input array representing the flow directions according to the D8
 *            model. Each cell value indicates the direction of flow according
 *            to the D8 convention.
 * @return    Array An array where each cell contains the number of incoming
 *            flow directions pointing to it.
 */
Array d8_compute_ndip(const Array &d8);

/**
 * @brief Identifies the indices of flow sinks within the heightmap.
 *
 * This function locates the cells in the heightmap that are flow sinks (cells
 * with no outflow) and returns their indices. Flow sinks are cells that do not
 * direct flow to any other cell.
 *
 * @param z  Input array representing the heightmap values.
 * @param is Output vector containing the row indices `i` of the flow sinks.
 * @param js Output vector containing the column indices `j` of the flow sinks.
 *
 * **Example**
 * @include ex_find_flow_sinks.cpp
 */
void find_flow_sinks(const Array      &z,
                     std::vector<int> &is,
                     std::vector<int> &js);

Array flooding_from_point(const Array &z, const int i, const int j);

Array flooding_from_point(const Array            &z,
                          const std::vector<int> &i,
                          const std::vector<int> &j);

/**
 * @brief Computes the flow accumulation for each cell using the D8 flow
 * direction model.
 *
 * This function calculates the flow accumulation for each cell in the heightmap
 * based on the D8 flow direction model \cite Zhou2019. Flow accumulation
 * represents the total amount of flow that accumulates at each cell from
 * upstream cells.
 *
 * @param  z Input array representing the heightmap values.
 * @return   Array An array where each cell contains the computed flow
 *           accumulation.
 *
 * **Example**
 * @include ex_flow_accumulation_d8.cpp
 *
 * **Result**
 * @image html ex_flow_accumulation_d80.png
 * @image html ex_flow_accumulation_d81.png
 *
 * @see      flow_direction_d8
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
 * @param  z         Input array representing the heightmap values.
 * @param  talus_ref Reference talus used to locally define the flow-partition
 *                   exponent. Small values will result in thinner flow streams.
 * @return           Array An array where each cell contains the computed flow
 *                   accumulation.
 *
 * **Example**
 * @include ex_flow_accumulation_dinf.cpp
 *
 * **Result**
 * @image html ex_flow_accumulation_dinf0.png
 * @image html ex_flow_accumulation_dinf1.png
 *
 * @see              flow_direction_dinf, flow_accumulation_d8
 */
Array flow_accumulation_dinf(const Array &z, float talus_ref);

/**
 * @brief Computes the flow direction from each cell to its downslope neighbor
 * using the D8 model.
 *
 * This function calculates the direction of flow for each cell in the heightmap
 * using the D8 flow direction model @cite Greenlee1987. The D8 model defines
 * eight possible flow directions as follows:
 * @verbatim 5 6 7 4 . 0 3 2 1
 * @endverbatim
 *
 * @param  z Input array representing the heightmap values.
 * @return   Array An array where each cell contains the flow direction
 *           according to the D8 nomenclature.
 *
 * @see      flow_accumulation_d8
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
 * @param  z         Input array representing the heightmap values.
 * @param  talus_ref Reference talus used to locally define the flow-partition
 *                   exponent. Smaller values will result in thinner flow
 *                   streams.
 * @return           std::vector<Array> A vector of arrays, each containing the
 *                   weights for flow directions at every cell.
 */
std::vector<Array> flow_direction_dinf(const Array &z, float talus_ref);

/**
 * @brief Computes the optimal flow path from a starting point to the boundary
 * of a given elevation array.
 *
 * This function finds the flow path on a grid represented by the input array
 * `z`, starting from the given point `ij_start`. It identifies the best path to
 * the boundary, minimizing upward elevation penalties while accounting for
 * distance and elevation factors.
 *
 * @param  z                   The input 2D array representing elevation values.
 * @param  ij_start            The starting point as a 2D vector of indices (i,
 *                             j) within the array.
 * @param  elevation_ratio     Weight for elevation difference in the cost
 *                             function (default: 0.5).
 * @param  distance_exponent   Exponent for the distance term in the cost
 *                             function (default: 2.0).
 * @param  upward_penalization Penalty factor for upward elevation changes
 *                             (default: 100.0).
 * @return                     A Path object representing the optimal flow path
 *                             with normalized x and y coordinates and
 *                             corresponding elevations.
 *
 * The output path consists of:
 * - Normalized x-coordinates along the path.
 * - Normalized y-coordinates along the path.
 * - Elevation values corresponding to each point on the path.
 *
 * **Example**
 * @include ex_flow_stream.cpp
 *
 * **Result**
 * @image html ex_flow_stream.png
 */
Path flow_stream(const Array    &z,
                 const Vec2<int> ij_start,
                 const float     elevation_ratio = 0.5f,
                 const float     distance_exponent = 2.f,
                 const float     upward_penalization = 100.f);

/**
 * @brief Generates a 2D array representing a riverbed based on a specified
 * path.
 *
 * This function calculates a scalar depth field (`dz`) for a riverbed shape
 * using a path, which can optionally be smoothed with Bezier curves. It
 * supports noise perturbation and post-filtering to adjust the riverbed's
 * features.
 *
 * @param  path                 The input path defining the riverbed's
 *                              trajectory.
 * @param  shape                The dimensions of the output array (width,
 *                              height).
 * @param  bbox                 The bounding box for the output grid in world
 *                              coordinates.
 * @param  bezier_smoothing     Flag to enable or disable Bezier smoothing of
 *                              the path.
 * @param  depth_start          The depth at the start of the riverbed.
 * @param  depth_end            The depth at the end of the riverbed.
 * @param  slope_start          The slope multiplier at the start of the
 *                              riverbed.
 * @param  slope_end            The slope multiplier at the end of the riverbed.
 * @param  shape_exponent_start The shape exponent at the start of the riverbed.
 * @param  shape_exponent_end   The shape exponent at the end of the riverbed.
 * @param  k_smoothing          The smoothing factor for the riverbed shape
 *                              adjustments.
 * @param  post_filter_ir       The radius of the post-filtering operation for
 *                              smoothing the output.
 * @param  p_noise_x            Optional pointer to a noise array for perturbing
 *                              the x-coordinates.
 * @param  p_noise_y            Optional pointer to a noise array for perturbing
 *                              the y-coordinates.
 * @param  p_noise_r            Optional pointer to a noise array for perturbing
 *                              the radial function.
 * @return                      A 2D array representing the calculated riverbed
 *                              depth field.
 *
 * @note The function requires the path to have at least two points. If the path
 * has fewer points, an empty array is returned with the given shape.
 *
 * **Example**
 * @include ex_generate_riverbed.cpp
 *
 * **Result**
 * @image html ex_generate_riverbed.png
 */
Array generate_riverbed(const Path &path,
                        Vec2<int>   shape,
                        Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f},
                        bool        bezier_smoothing = false,
                        float       depth_start = 0.01f,
                        float       depth_end = 1.f,
                        float       slope_start = 64.f,
                        float       slope_end = 32.f,
                        float       shape_exponent_start = 1.f,
                        float       shape_exponent_end = 10.f,
                        float       k_smoothing = 0.5f,
                        int         post_filter_ir = 0,
                        Array      *p_noise_x = nullptr,
                        Array      *p_noise_y = nullptr,
                        Array      *p_noise_r = nullptr);

} // namespace hmap

namespace hmap::gpu
{

/*! @brief See hmap::flow_direction_d8 */
Array flow_direction_d8(const Array &z);

/*! @brief See hmap::generate_riverbed */
Array generate_riverbed(const Path &path,
                        Vec2<int>   shape,
                        Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f},
                        bool        bezier_smoothing = false,
                        float       depth_start = 0.01f,
                        float       depth_end = 1.f,
                        float       slope_start = 64.f,
                        float       slope_end = 32.f,
                        float       shape_exponent_start = 1.f,
                        float       shape_exponent_end = 10.f,
                        float       k_smoothing = 0.5f,
                        int         post_filter_ir = 0,
                        Array      *p_noise_x = nullptr,
                        Array      *p_noise_y = nullptr,
                        Array      *p_noise_r = nullptr);

} // namespace hmap::gpu