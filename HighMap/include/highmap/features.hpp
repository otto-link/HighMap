/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file op.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace hmap
{

/**
 * @brief Return the connected-component labelling of the array.
 *
 * See https://en.wikipedia.org/wiki/Connected-component_labeling.
 *
 * @param array Input array.
 * @param surface_threshold Surface threshold (in number of pixels): if not set
 * to zero, components with a surface smaller than the threshold are removed.
 * @param background_value Background value.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_connected_components.cpp
 *
 * **Result**
 * @image html ex_connected_components0.png
 * @image html ex_connected_components1.png
 */
Array connected_components(const Array &array,
                           float        surface_threshold = 0.f,
                           float        background_value = 0.f);

/**
 * @brief Return the Gaussian curvature @cite Kurita1992.
 *
 * @param z Input array.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_curvature_gaussian.cpp
 *
 * **Result**
 * @image html ex_curvature_gaussian0.png
 * @image html ex_curvature_gaussian1.png
 * @image html ex_curvature_gaussian2.png
 * @image html ex_curvature_gaussian3.png
 * @image html ex_curvature_gaussian4.png
 */
Array curvature_gaussian(const Array &z);

/**
 * @brief Return the mean curvature @cite Kurita1992.
 *
 * @param z Input array.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_curvature_gaussian.cpp
 *
 * **Result**
 * @image html ex_curvature_gaussian0.png
 * @image html ex_curvature_gaussian1.png
 * @image html ex_curvature_gaussian2.png
 * @image html ex_curvature_gaussian3.png
 * @image html ex_curvature_gaussian4.png
 */
Array curvature_mean(const Array &z);

/**
 * @brief Return the labelling of a geomorphons-based classification @cite
 * Jasiewicz2013.
 *
 * @param array Input array.
 * @param irmin Minimum lookup radius (in pixels).
 * @param irmax Maximum lookup radius (in pixels).
 * @param epsilon Slope tolerance defining 'flatness'.
 * @return Array Output array.
 *
 * **Example**
 * @include ex_geomorphons.cpp
 *
 * **Result**
 * @image html ex_geomorphons0.png
 * @image html ex_geomorphons1.png
 */
Array geomorphons(const Array &array, int irmin, int irmax, float epsilon);

/**
 * @brief Return the labelling of each cell of the array based on a k-means
 * clustering, with two arrays of input data.
 *
 * @param array1 Input array #1.
 * @param array2 Input array #2.
 * @param nclusters Number of clusters.
 * @param weights Feature weights.
 * @param seed Random seed number.
 * @return Array Resulting label array.
 *
 * **Example**
 * @include ex_kmeans_clustering.cpp
 *
 * **Result**
 * @image html ex_kmeans_clustering0.png
 * @image html ex_kmeans_clustering1.png
 * @image html ex_kmeans_clustering2.png
 */
Array kmeans_clustering2(const Array &array1,
                         const Array &array2,
                         int          nclusters,
                         Vec2<float>  weights = {1.f, 1.f},
                         uint         seed = 1);

Array kmeans_clustering3(const Array &array1,
                         const Array &array2,
                         const Array &array3,
                         int          nclusters,
                         Vec3<float>  weights = {1.f, 1.f, 1.f},
                         uint         seed = 1); ///< @overload

/**
 * @brief Return the relative elevation within a a radius `ir`.
 *
 * @param array Input array.
 * @param ir Lookup radius, in pixels.
 * @return Array Relative elevation, in [0, 1].
 *
 * **Example**
 * @include relative_elevation.cpp
 *
 * **Result**
 * @image html relative_elevation.png
 */
Array relative_elevation(const Array &array, int ir);

/**
 * @brief Return rugosity estimate (based on the skewness).
 *
 * @param z Input array.
 * @param ir Square kernel footprint radius.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_rugosity.cpp
 *
 * **Result**
 * @image html ex_rugosity0.png
 * @image html ex_rugosity1.png
 */
Array rugosity(const Array &z, int ir);

/**
 * @brief The Shape Index (SI) is a measure used to quantify the shape
 * complexity of landforms in a DEM. It is calculated based on the second
 * derivatives of the elevation surface.
 *
 * See @cite Florinsky2011
 *
 * @param z Input array.
 * @param ir Pre-filter radius.
 * @return Resulting array (> 0.5 for convex and < 0.5 for concave).
 *
 * **Example**
 * @include ex_curvature_gaussian.cpp
 *
 * **Result**
 * @image html ex_curvature_gaussian0.png
 * @image html ex_curvature_gaussian1.png
 * @image html ex_curvature_gaussian2.png
 * @image html ex_curvature_gaussian3.png
 * @image html ex_curvature_gaussian4.png
 */
Array shape_index(const Array &z, int ir);

/**
 * @brief Unsesphericity refers to the degree to which an object or surface
 * deviates from being perfectly spherical or symmetrical.
 * @param z Input array.
 * @param ir Pre-filter radius.
 * @return Resulting array.
 * @return Resulting array (> 0.5 for convex and < 0.5 for concave).
 *
 * **Example**
 * @include ex_curvature_gaussian.cpp
 *
 * **Result**
 * @image html ex_curvature_gaussian0.png
 * @image html ex_curvature_gaussian1.png
 * @image html ex_curvature_gaussian2.png
 * @image html ex_curvature_gaussian3.png
 * @image html ex_curvature_gaussian4.png
 */
Array unsphericity(const Array &z, int ir);

/**
 * @brief Return the "valley width", corresponding to the distance to the
 * concave region frontier (in this concave frontier).
 *
 * @param z Input array.
 * @param ir Pre-filter radius.
 * @return Array Resulting array.
 *
 * **Example**
 * @include ex_valley_width.cpp
 *
 * **Result**
 * @image html ex_valley_width0.png
 * @image html ex_valley_width1.png
 */
Array valley_width(const Array &z, int ir = 0);

} // namespace hmap