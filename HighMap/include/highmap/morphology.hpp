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
 */
Array distance_transform(const Array &array,
                         bool         return_squared_distance = false);

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

} // namespace hmap