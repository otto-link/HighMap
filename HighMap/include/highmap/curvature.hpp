/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file curvature.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file defining a collection of functions for curvature analysis.
 *
 * @copyright Copyright (c) 2023 Otto Link
 */
#pragma once

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief Computes the accumulation curvature of a heightmap. Acumulation
 * curvature is a measure of the extent of local accumulation of flows at a
 * given point.
 *
 * @param  z  The input array representing the heightmap data (elevation
 *            values).
 * @param  ir The radius used for pre-filtering, which controls the scale of the
 *            analysis (in pixels).
 * @return    Array An output array containing the calculated accumulation
 *            curvature values for each point in the input heightmap.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array accumulation_curvature(const Array &z, int ir);

/**
 * @brief Calculates the Gaussian curvature of a heightmap, providing insights
 * into the surface's intrinsic curvature at each point. Gaussian curvature is a
 * fundamental measure of surface curvature, indicating how the surface bends in
 * multiple directions at each point. This metric is often used in geomorphology
 * to understand landform shapes. Usage: Use this function to analyze the
 * overall shape of terrain features, identifying whether regions are
 * saddle-like, dome-like, or basin-like. Useful in studies related to
 * tectonics, erosion patterns, and landform development.
 *
 * @param  z The input array representing the heightmap data (elevation values).
 * @return   Array An output array containing the Gaussian curvature values,
 *           with positive values indicating dome-like shapes and negative
 *           values indicating saddle shapes.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array curvature_gaussian(const Array &z);

/**
 * @brief TODO
 *
 * @param  z The input array representing the heightmap data (elevation values).
 * @return   Array An output array containing the curvature values.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array curvature_horizontal_cross_sectional(const Array &z, int ir);

/**
 * @brief TODO
 *
 * @param  z The input array representing the heightmap data (elevation values).
 * @return   Array An output array containing the curvature values.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array curvature_horizontal_plan(const Array &z, int ir);

/**
 * @brief TODO
 *
 * @param  z The input array representing the heightmap data (elevation values).
 * @return   Array An output array containing the curvature values.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array curvature_horizontal_tangential(const Array &z, int ir);

/**
 * @brief Computes the mean curvature of a heightmap, indicating the average
 * curvature at each point on the surface. Mean curvature is another critical
 * metric in geomorphology, representing the average bending of the surface.
 * This measure is useful in understanding terrain smoothness and can help
 * identify areas of potential erosion or deposition. Usage: Apply this function
 * to detect areas prone to erosion or sediment deposition. Useful in landscape
 * evolution models and in analyzing the stability of slopes.
 *
 * @param  z The input array representing the heightmap data (elevation values).
 * @return   Array An output array containing the mean curvature values, where
 *           positive values indicate convex regions and negative values
 *           indicate concave regions.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array curvature_mean(const Array &z);

/**
 * @brief Ring curvature is a second-order derivative of the elevation surface.
 * It describes how the surface bends along a ring-like shape, often computed
 * from the principal curvatures. Positive Values: Indicate convex surfaces
 * where flow disperses (ridges, hilltops). Negative Values: Indicate concave
 * surfaces where flow converges (valleys, depressions).
 *
 * @param  z The input array representing the heightmap data (elevation values).
 * @return   Array An output array containing the curvature values.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array curvature_ring(const Array &z, int ir);

/**
 * @brief Rotor curvature, also called flow line curvature, describes how the
 * curvature of a terrain surface influences the acceleration or deceleration of
 * flow (e.g., water, debris, or air) along the direction of maximum slope.
 * Positive values: Flow is decelerating (convex-up terrain, such as ridges or
 * crests). Negative values: Flow is accelerating (concave-down terrain, such as
 * valleys or channels). Zero values: Flow moves in a linear, constant-slope
 * manner.
 *
 * @param  z The input array representing the heightmap data (elevation values).
 * @return   Array An output array containing the curvature values.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array curvature_rotor(const Array &z, int ir);

/**
 * @brief TODO
 *
 * @param  z The input array representing the heightmap data (elevation values).
 * @return   Array An output array containing the curvature values.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array curvature_vertical_longitudinal(const Array &z, int ir);

/**
 * @brief TODO
 *
 * @param  z The input array representing the heightmap data (elevation values).
 * @return   Array An output array containing the curvature values.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array curvature_vertical_profile(const Array &z, int ir);

/**
 * @brief Computes the Shape Index (SI) of the terrain, quantifying landform
 * complexity based on curvature. The Shape Index is a metric used to describe
 * the shape of landforms, particularly in digital elevation models (DEMs). It
 * differentiates between convex (e.g., hilltops), concave (e.g., valleys), and
 * flat surfaces. Usage: Use this function to classify terrain into different
 * morphological types, which can be important in land use planning and
 * environmental studies. Useful in landscape ecology and in understanding
 * geomorphological processes.
 *
 * @param  z  The input array representing the heightmap data (elevation
 *            values).
 * @param  ir The radius used for pre-filtering, which controls the scale of the
 *            analysis (in pixels).
 * @return    Array An output array containing Shape Index values, where values
 *            above 0.5 indicate convex shapes, and values below 0.5 indicate
 *            concave shapes.
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array shape_index(const Array &z, int ir);

/**
 * @brief Calculates the unsphericity of a surface, indicating how much the
 * terrain deviates from a perfect spherical shape. Unsphericity is a measure
 * used to understand the degree of asymmetry in terrain surfaces. It quantifies
 * how much a surface deviates from being perfectly spherical or symmetrical,
 * which can be critical in various geomorphological analyses. Usage: Use this
 * function to identify areas of terrain that significantly deviate from a
 * spherical shape, which may indicate unique geological formations or erosion
 * patterns. Helpful in identifying and analyzing landforms that are not
 * perfectly round or symmetrical, such as irregular hills or basins.
 *
 * @param  z  The input array representing the heightmap data (elevation
 *            values).
 * @param  ir The radius used for pre-filtering, controlling the scale of
 *            analysis (in pixels).
 * @return    Array An output array containing unsphericity values, where values
 *            greater than 0.5 indicate convex regions (e.g., peaks) and values
 *            less than 0.5 indicate concave regions (e.g., valleys).
 *
 * **Example**
 * @include ex_curvature.cpp
 *
 * **Result**
 * @image html ex_curvature.png
 */
Array unsphericity(const Array &z, int ir);

// helpers

// Notation taken from Florinsky, I. (2016). Digital terrain analysis in soil
// science and geology. Academic Press. p = dz/dx, q = dz/dy, r = d2z/dx2, s =
// d2z/dxdy, t = d2z/dy2.
void compute_curvature_gradients(const Array &z,
                                 Array       &p,
                                 Array       &q,
                                 Array       &r,
                                 Array       &s,
                                 Array       &t);

Array compute_curvature_h(const Array &r, const Array &t);

Array compute_curvature_k(const Array &p,
                          const Array &q,
                          const Array &r,
                          const Array &s,
                          const Array &t);

} // namespace hmap

namespace hmap::gpu
{

/*! @brief See hmap::accumulation_curvature */
Array accumulation_curvature(const Array &z, int ir);

/*! @brief See hmap::curvature_horizontal_cross_sectional */
Array curvature_horizontal_cross_sectional(const Array &z, int ir);

/*! @brief See hmap::curvature_horizontal_plan */
Array curvature_horizontal_plan(const Array &z, int ir);

/*! @brief See hmap::curvature_horizontal_tangential */
Array curvature_horizontal_tangential(const Array &z, int ir);

/*! @brief See hmap::curvature_ring */
Array curvature_ring(const Array &z, int ir);

/*! @brief See hmap::curvature_rotor */
Array curvature_rotor(const Array &z, int ir);

/*! @brief See hmap::curvature_vertical_longitudinal */
Array curvature_vertical_longitudinal(const Array &z, int ir);

/*! @brief See hmap::curvature_vertical_profile */
Array curvature_vertical_profile(const Array &z, int ir);

/*! @brief See hmap::shape_index */
Array shape_index(const Array &z, int ir);

/*! @brief See hmap::unsphericity */
Array unsphericity(const Array &z, int ir);

} // namespace hmap::gpu