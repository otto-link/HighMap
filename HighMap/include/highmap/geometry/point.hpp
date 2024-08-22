/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file point.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Defines a class for representing and manipulating 3D points.
 *
 * This file contains the definition of the `Point3` class, which represents a
 * point in 2D space with `x`, `y`, carrying a vlue `v`. The class includes
 * basic vector operations such as addition, subtraction, and scalar
 * multiplication. It also provides functionality for linear interpolation
 * between two points.
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

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @class Point
 * @brief A class to represent and manipulate 2D points that can carry a value.
 */
class Point
{
public:
  float x; ///< The x-coordinate of the point.
  float y; ///< The y-coordinate of the point.
  float v; ///< The value at the point.

  /**
   * @brief Default constructor initializing the point to (0, 0, 0).
   */
  Point() : x(0.f), y(0.f), v(0.f)
  {
  }

  /**
   * @brief Parameterized constructor initializing the point to given values.
   * @param x The x-coordinate of the point.
   * @param y The y-coordinate of the point.
   * @param v The value at the point.
   */
  Point(float x, float y, float v = 0.f) : x(x), y(y), v(v)
  {
  }

  /**
   * @brief Equality operator to check if two points are the same.
   *
   * @param other The point to compare with.
   * @return true if the points are equal, false otherwise.
   */
  bool operator==(const Point &other) const
  {
    return (x == other.x && y == other.y && v == other.v);
  }

  /**
   * @brief Inequality operator to check if two points are different.
   *
   * @param other The point to compare with.
   * @return true if the points are not equal, false otherwise.
   */
  bool operator!=(const Point &other) const
  {
    return !(*this == other);
  }

  /**
   * @brief Adds two points.
   * @param other The point to add.
   * @return The result of adding the two points.
   */
  Point operator+(const Point &other) const
  {
    return Point(x + other.x, y + other.y, v + other.v);
  }

  /**
   * @brief Subtracts two points.
   * @param other The point to subtract.
   * @return The result of subtracting the other point from this point.
   */
  Point operator-(const Point &other) const
  {
    return Point(x - other.x, y - other.y, v - other.v);
  }

  /**
   * @brief Multiplies the point by a scalar.
   * @param scalar The scalar to multiply by.
   * @return The result of the multiplication.
   */
  Point operator*(float scalar) const
  {
    return Point(x * scalar, y * scalar, v * scalar);
  }

  /**
   * @brief Divides the point by a scalar.
   * @param scalar The scalar to divide by.
   * @return The result of the division.
   */
  Point operator/(float scalar) const
  {
    return Point(x / scalar, y / scalar, v / scalar);
  }

  /**
   * @brief Scalar multiplication (scalar * Vec2).
   *
   * Multiplies each component of the vector by a scalar value. This function
   * allows expressions where the scalar is on the left side of the
   * multiplication operator.
   *
   * @param scalar The scalar value to multiply with.
   * @param point The vector to multiply.
   * @return Point A new vector with each component multiplied by the scalar.
   */
  friend Point operator*(float scalar, const Point &point)
  {
    return Point(scalar * point.x, scalar * point.y, scalar * point.v);
  }

  /**
   * @brief Set the point value by interpolating values of an array at the point
   * position.
   * @param array Input array.
   * @param bbox Array bounding box of the area, used to draw a correspondance
   * between point position and array cells positions.
   */
  void set_value_from_array(const Array &array, Vec4<float> bbox);

  /**
   * @brief Updates the point's value based on bilinear interpolation from an
   * array.
   *
   * This function updates the value of the `Point` object by performing
   * bilinear interpolation on the input `Array` using the coordinates of the
   * `Point` and a given bounding box. The point's coordinates are first
   * normalized to the unit interval using the provided bounding box. Then,
   * these normalized coordinates are scaled to the array's dimensions and used
   * to fetch the value from the array through bilinear interpolation.
   *
   * If the normalized coordinates fall outside the bounds of the array, the
   * point's value is set to zero.
   *
   * @param array The input `Array` from which the value is interpolated. The
   * `Array` should support bilinear interpolation.
   * @param bbox Bounding box used for normalizing the `Point`'s coordinates.
   * This box is defined by a `Vec4<float>` containing minimum and maximum
   * values for both x and y dimensions in the format `{xmin, xmax, ymin,
   * ymax}`.
   *
   * @note If the coordinates are outside the bounds of the array after scaling,
   * the point's value is set to zero.
   */
  void update_value_from_array(const Array &array, Vec4<float> bbox);
};

/**
 * @brief Computes the angle between two points relative to the x-axis.
 *
 * This function calculates the angle formed by the vector from `p1` to `p2`
 * with respect to the x-axis. The angle is measured in radians and is
 * oriented in the counter-clockwise direction.
 *
 * @param p1 The starting point of the vector.
 * @param p2 The ending point of the vector.
 * @return The angle in radians between the vector formed by `p1` to `p2` and
 *         the x-axis. The angle is in the range [-π, π].
 */
float angle(const Point &p1, const Point &p2);

/**
 * @brief Computes the angle formed by three points with the reference point as
 * the origin.
 *
 * Given three points \( p0 \), \( p1 \), and \( p2 \), this function calculates
 * the angle formed between the vectors \( p0 \rightarrow p2 \) and \( p0
 * \rightarrow p1 \). The angle is oriented in the 2D plane and measured in
 * radians.
 *
 * @param p0 The reference point (origin of the angle measurement).
 * @param p1 The first point defining the angle.
 * @param p2 The second point defining the angle.
 * @return The angle between the vectors \( p0 \rightarrow p2 \) and \( p0
 * \rightarrow p1 \) in radians.
 *
 * @note The angle is calculated using the 2D vectors defined by \( p0
 * \rightarrow p1 \) and \( p0 \rightarrow p2 \). The result will be in the
 * range \([-π, π]\).
 */
float angle(const Point &p0, const Point &p1, const Point &p2);

/**
 * @brief Calculates the distance between two points.
 *
 * This function computes the Euclidean distance between two points
 * in 3D space.
 *
 * @param p1 The first point.
 * @param p2 The second point.
 * @return The distance between the two points.
 */
float distance(const Point &p1, const Point &p2);

/**
 * @brief Linearly interpolates between two points.
 *
 * This function performs linear interpolation between two points based on a
 * given factor. The interpolation factor `t` should be in the range [0, 1],
 * where 0 corresponds to the first point and 1 corresponds to the second point.
 *
 * @param p1 The starting point.
 * @param p2 The ending point.
 * @param t The interpolation factor (0 <= t <= 1).
 * @return The interpolated point between `p1` and `p2`.
 *
 * @note If `t` is outside the range [0, 1], the function will still return a
 * point outside the segment defined by `p1` and `p2`.
 */
Point lerp(const Point &p1, const Point &p2, float t);

/**
 * @brief Sorts a vector of points in ascending order based on their
 * coordinates.
 *
 * This function sorts the provided vector of `Point` objects first by the
 * x-coordinate and then by the y-coordinate in ascending order. The sorting is
 * done in-place.
 *
 * @param points A vector of `Point` objects to be sorted. The vector is
 * modified directly with the points arranged in ascending order.
 */
void sort_points(std::vector<Point> &points);

} // namespace hmap