/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file edge.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief This file defines the `Edge` class for manipulating segments in 2D
 * space.
 *
 * The `Edge` class represents a line segment in 2D space, defined by its start
 * and end points. It provides methods to initialize an edge and compute its
 * length.
 *
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 * This software is distributed under the terms of the GNU General Public
 * License. The full license is in the file LICENSE, distributed with this
 * software.
 */
#pragma once
#include <cmath>

#include "highmap/geometry/point.hpp"

namespace hmap
{

/**
 * @brief Represents a line segment in 2D space.
 *
 * The `Edge` class defines a line segment using two `Point` objects to
 * represent the start and end of the segment. Additionally, it has a weight
 * associated with the edge, and provides functionality to compute the length of
 * the segment.
 */
class Edge
{
public:
  Point p1; ///< The start point of the edge.
  Point p2; ///< The end point of the edge.
  float w;  ///< A weight associated with the edge.

  /**
   * @brief Constructs an `Edge` object with specified start and end points and
   * a weight.
   *
   * @param p1 The start point of the edge.
   * @param p2 The end point of the edge.
   * @param w The weight associated with the edge.
   */
  Edge(Point p1, Point p2, float w) : p1(p1), p2(p2), w(w)
  {
  }

  /**
   * @brief Constructs an `Edge` object with specified start and end points and
   * a default weight of 0.
   *
   * @param p1 The start point of the edge.
   * @param p2 The end point of the edge.
   */
  Edge(Point p1, Point p2) : p1(p1), p2(p2), w(0)
  {
  }

  /**
   * @brief Computes the length of the edge.
   *
   * This function calculates the distance between the start and end points of
   * the edge.
   *
   * @return The length of the edge.
   */
  float length()
  {
    return distance(p1, p2);
  }
};

} // namespace hmap