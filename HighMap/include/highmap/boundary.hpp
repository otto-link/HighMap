/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file boundary.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for boundary condition functions and utilities.
 *
 * This header file contains function declarations and utilities for managing
 * heightmap values at the boundary conditions.
 *
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once
#include "highmap/array.hpp"
#include "highmap/math.hpp"

namespace hmap
{

/**
 * @brief Linear extrapolation of values at the borders (i = 0, j = 0, ...)
 * based on inner values.
 *
 * @param array Input array.
 * @param nbuffer Buffer depth.
 * @param sigma Relaxation coefficient.
 *
 * @see {@link fill_borders}
 *
 */
void extrapolate_borders(Array &array, int nbuffer = 1, float sigma = 0.f);

/**
 * @brief Applies a falloff effect to the input array based on distance.
 *
 * This function modifies the elements of the input `array` by applying a
 * falloff effect, which decreases the values based on their distance from a
 * central point or area. The strength of the falloff, the type of distance
 * function used, and optional noise can be specified.
 *
 * @param array Reference to the array that will be modified by the falloff
 * effect.
 * @param strength The strength of the falloff effect. A higher value results in
 * a stronger falloff. Default is 1.0f.
 * @param dist_fct The distance function to be used for calculating the falloff.
 *        Options include Euclidian and others, with
 * `DistanceFunction::EUCLIDIAN` as the default.
 * @param p_noise Optional pointer to an array that provides noise to be added
 *        to the falloff effect. If nullptr (default), no noise is added.
 * @param bbox A 4D vector representing the bounding box within which the
 * falloff effect is applied. The default is {0.f, 1.f, 0.f, 1.f}.
 *
 * **Example**
 * @include ex_falloff.cpp
 *
 * **Result**
 * @image html ex_falloff.png
 */
void falloff(Array           &array,
             float            strength = 1.f,
             DistanceFunction dist_fct = DistanceFunction::EUCLIDIAN,
             Array           *p_noise = nullptr,
             Vec4<float>      bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Fill values at the borders (i = 0, j = 0, ...) based on 1st neighbor
 * values.
 *
 * @param array Input array.
 *
 * @see {@link extrapolate_borders}
 */
void fill_borders(Array &array);
/**
 * @brief Return an array with buffers at the boundaries (values filled by
 * symmetry).
 *
 * @param array Input array.
 * @param buffers Buffer size {east, west, south, north}.
 * @param zero_padding Use zero-padding instead of symmetry to fill values.
 * @return Array New array with buffers.
 */
Array generate_buffered_array(const Array &array,
                              Vec4<int>    buffers,
                              bool         zero_padding = false);

/**
 * @brief Make the array periodic in both directions.
 *
 * @param array Input array.
 * @param nbuffer Transition width at the boundaries.
 *
 * **Example**
 * @include ex_make_periodic.cpp
 *
 * **Result**
 * @image html ex_make_periodic0.png
 * @image html ex_make_periodic1.png
 */
void make_periodic(Array &array, int nbuffer);

/**
 * @brief Make the array periodic in both directions using a stitching operation
 * minimizing errors.
 *
 * @param array Input array.
 * @param Overlap overlap based on domain half size (if overlap = 1, the
 * transition, made on both sides, then spans the whole domain).
 * @return Array Periodic array.
 *
 * **Example**
 * @include ex_make_periodic_stitching.cpp
 *
 * **Result**
 * @image html ex_make_periodic_stitching0.png
 * @image html ex_make_periodic_stitching1.png
 */
Array make_periodic_stitching(Array &array, float overlap);

/**
 * @brief
 *
 * @param array Input array.
 * @param Overlap overlap based on domain half size (if overlap = 1, the
 * transition, made on both sides, then spans the whole domain).
 * @param tiling Array tiling.
 * @return Tiled array.
 *
 * **Example**
 * @include make_periodic_tiling.cpp
 *
 * **Result**
 * @image html make_periodic_tiling.png
 */
Array make_periodic_tiling(Array &array, float overlap, Vec2<int> tiling);

/**
 * @brief Enforce values at the boundaries of the array.
 *
 * @param array Input array.
 * @param values Value at the borders {east, west, south, north}.
 * @param buffer_sizes Buffer size at the borders {east, west, south, north}.
 *
 * **Example**
 * @include ex_set_borders.cpp
 *
 * **Result**
 * @image html ex_set_borders.png
 */
void set_borders(Array      &array,
                 Vec4<float> border_values,
                 Vec4<int>   buffer_sizes);

void set_borders(Array &array,
                 float  border_values,
                 int    buffer_sizes); ///< @overload

/**
 * @brief Use symmetry for to fill values at the domain borders, over a given
 * buffer depth.
 *
 * @param array Input array.
 * @param buffer_sizes Buffer size at the borders {east, west, south, north}.
 */
void sym_borders(Array &array, Vec4<int> buffer_sizes);

/**
 * @brief Fill values at the borders (i = 0, j = 0, ...) with zeros.
 *
 * @param array Input array.
 *
 * @see {@link fill_borders}
 */
void zeroed_borders(Array &array);

/**
 * @brief Apply a smooth transition to zero at the array borders.
 *
 * @param array Input array.
 * @param sigma Transition half-width ratio.
 * @param p_noise Reference to the input noise arrays.
 * @param bbox Domain bounding box.
 *
 * **Example**
 * @include ex_zeroed_edges.cpp
 *
 * **Result**
 * @image html ex_zeroed_edges.png
 */
void zeroed_edges(Array           &array,
                  float            sigma = 1.f,
                  DistanceFunction dist_fct = DistanceFunction::EUCLIDIAN,
                  Array           *p_noise = nullptr,
                  Vec4<float>      bbox = {0.f, 1.f, 0.f, 1.f});

} // namespace hmap