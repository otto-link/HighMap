/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file morphology.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for image morphology functions and operations.
 *
 * This header file provides declarations for functions and utilities related
 * to image morphology, including operations such as dilation, erosion,
 * opening, closing, etc. These functions are commonly used in image processing
 * to enhance or extract features based on the shape and structure of objects
 * within an image.
 *
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Apply a closing algorithm to the input array using a square
 * structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array closing(const Array &array, int ir);

/**
 * @brief Apply a dilation algorithm to the input array using a square
 * structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array dilation(const Array &array, int ir);

/**
 * @brief Return the Euclidean distance transform.
 *
 * Exact transform based on Meijster et al. algorithm @cite Meijster2000.
 *
 * @param array Input array to be transformed, will be converted into binary: 1
 * wherever input is greater than 0, 0 elsewhere.
 * @param return_squared_distance Wheter the distance returned is squared or
 * not.
 * @return Array Reference to the output array.
 *
 * **Example**
 * @include ex_distance_transform.cpp
 *
 * **Result**
 * @image html ex_distance_transform0.png
 * @image html ex_distance_transform1.png
 * @image html ex_distance_transform2.png
 */
Array distance_transform(const Array &array,
                         bool         return_squared_distance = false);

/**
 * @brief Calculates an approximate distance transform of the input array.
 *
 * @param array Input array to calculate the distance transform for.
 * @param return_squared_distance Optional parameter to return squared
 * distances. Defaults to false.
 *
 * @return Array containing the approximate distance transform of the input
 * array.
 *
 * **Example**
 * @include ex_distance_transform.cpp
 *
 * **Result**
 * @image html ex_distance_transform0.png
 * @image html ex_distance_transform1.png
 * @image html ex_distance_transform2.png

 */
Array distance_transform_approx(const Array &array,
                                bool         return_squared_distance = false);

/**
 * @brief Calculates the Manhattan distance transform of an array.
 *
 * @param array Input array.
 * @param return_squared_distance If true, returns the squared Manhattan
 * distance instead of the actual distance.
 * @return Array containing the Manhattan distance transform of the input array.
 *
 * **Example**
 * @include ex_distance_transform.cpp
 *
 * **Result**
 * @image html ex_distance_transform0.png
 * @image html ex_distance_transform1.png
 * @image html ex_distance_transform2.png
 */
Array distance_transform_manhattan(const Array &array,
                                   bool return_squared_distance = false);

/**
 * @brief Apply an erosion algorithm to the input array using a square
 * structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 *  **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array erosion(const Array &array, int ir);

/**
 * @brief Apply a flood fill algorithm to the input array.
 *
 * @param array Input array.
 * @param i Seed point index.
 * @param j Seed point index.
 * @param fill_value Filling value.
 * @param background_value Background value.
 *
 * **Example**
 * @include ex_flood_fill.cpp
 *
 * **Result**
 * @image html ex_flood_fill.png
 */
void flood_fill(Array &array,
                int    i,
                int    j,
                float  fill_value = 1.f,
                float  background_value = 0.f);

/**
 * @brief Apply a morphological_black_hat algorithm to the input array using a
 * square structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array morphological_black_hat(const Array &array, int ir);

/**
 * @brief Apply a morphological_gradient algorithm to the input array using a
 * square structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array morphological_gradient(const Array &array, int ir);

/**
 * @brief Apply a morphological_top_hat algorithm to the input array using a
 * square structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array morphological_top_hat(const Array &array, int ir);

/**
 * @brief Apply an opening algorithm to the input array using a square
 * structure.
 * @param array Input array.
 * @param ir Square kernel radius.
 * @return Output array.
 *
 * **Example**
 * @include ex_morphology_base.cpp
 *
 * **Result**
 * @image html ex_morphology_base.png
 */
Array opening(const Array &array, int ir);

/**
 * @brief Computes the relative distance of each non-zero cell in a binary array
 * from the skeleton and border.
 *
 * This function calculates a relative distance measure for each non-zero cell
 * in the input array. The measure is defined as the ratio of the cell's
 * distance to the nearest border and the combined distances to the nearest
 * skeleton and border cells. The skeleton is computed using the Zhang-Suen
 * skeletonization algorithm.
 *
 * @param array The input binary array for which the relative distance map is to
 * be calculated. Non-zero values are considered for processing.
 * @param ir_search The search radius for finding the nearest skeleton and
 * border cells.
 * @param zero_at_borders If true, the borders of the skeletonized image will be
 * set to zero.
 * @return An array representing the relative distance map, where each cell has
 * a value between 0 and 1. A value closer to 1 indicates proximity to the
 * skeleton, while a value closer to 0 indicates proximity to the border.
 *
 * @note The skeleton is computed using the Zhang-Suen skeletonization
 * algorithm.
 *
 * **Example**
 * @include ex_skeleton.cpp
 *
 * **Result**
 * @image html ex_skeleton.png
 */
Array relative_distance_from_skeleton(const Array &array,
                                      int          ir_search,
                                      bool         zero_at_borders = true,
                                      int          ir_erosion = 1);

/**
 * @brief Computes the skeleton of a binary image using the Zhang-Suen
 * skeletonization algorithm.
 *
 * This function processes a binary input array to extract its skeleton by
 * iteratively thinning the image until no further changes occur. It optionally
 * sets the borders of the resulting skeletonized image to zero.
 *
 * @param array The input binary array to be skeletonized. Values should
 * typically be 0 or 1.
 * @param zero_at_borders If true, the borders of the resulting array will be
 * set to zero.
 * @return The skeletonized version of the input array.
 *
 * @note This implementation is based on the algorithm described at
 *       https://github.com/krishraghuram/Zhang-Suen-Skeletonization.
 *
 * **Example**
 * @include ex_skeleton.cpp
 *
 * **Result**
 * @image html ex_skeleton.png
 */
Array skeleton(const Array &array, bool zero_at_borders = true);

} // namespace hmap