/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file cloud.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Definition of the `Cloud` class for manipulating sets of 2D points.
 *
 * This file contains the definition of the `Cloud` class, which is used to
 * manage and manipulate unordered sets of points in a 2D space. The `Cloud`
 * class provides functionalities for creating clouds of points, including
 * random generation, value assignment, and data export. It also supports
 * operations such as computing bounding boxes, distances, and interpolating
 * values from arrays.
 *
 * The class includes methods for adding and removing points, randomizing
 * positions and values, remapping values, and exporting data to CSV files.
 * Additional functionalities include calculating Signed Distance Functions
 * (SDF) and projecting point clouds onto arrays.
 *
 * @version 0.1 Initial version.
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023 Otto Link
 *
 * This software is distributed under the terms of the GNU General Public
 * License. The full license is available in the LICENSE file distributed with
 * this software.
 */
#pragma once
#include <cmath>

#include "highmap/array.hpp"
#include "highmap/geometry/point.hpp"
#include "highmap/interpolate2d.hpp"

namespace hmap
{

class Graph;

/**
 * @class Cloud
 * @brief Represents a collection of unordered points in 2D space.
 *
 * The `Cloud` class provides functionality to manage and manipulate an
 * unordered set of points in a 2D space. It supports various operations such as
 * adding points, calculating the centroid, merging with other point clouds, and
 * more. This class is useful for applications involving point cloud processing,
 * geometric computations, and spatial analysis.
 */
class Cloud
{
public:
  std::vector<Point> points = {}; ///< Points of the cloud.

  /**
   * @brief Default constructor for the Cloud class.
   *
   * Initializes an empty cloud with no points.
   */
  Cloud(){};

  /**
   * @brief Constructs a new Cloud object with random positions and values.
   *
   * This constructor generates a cloud with a specified number of points, where
   * the positions and values of the points are randomly generated within a
   * given bounding box.
   *
   * @param npoints Number of points to generate.
   * @param seed Random seed used to generate the points.
   *             Using the same seed will produce the same set of points.
   * @param bbox Bounding box within which the points will be generated.
   *             The bounding box is defined as {xmin, xmax, ymin, ymax}.
   */
  Cloud(int npoints, uint seed, Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Constructs a new Cloud object based on a list of existing points.
   *
   * This constructor initializes the cloud with a pre-defined set of points,
   * which are passed as a vector of `Point` objects.
   *
   * @param points A vector of `Point` objects representing the cloud's points.
   */
  Cloud(std::vector<Point> points) : points(points){};

  /**
   * @brief Constructs a new Cloud object from lists of `x` and `y` coordinates.
   *
   * This constructor allows the creation of a cloud by providing separate lists
   * of `x` and `y` coordinates. Each point will have an associated value,
   * which is set to a default value.
   *
   * @param x A vector of `x` coordinates for the points.
   * @param y A vector of `y` coordinates for the points.
   * @param default_value The default value assigned to each point.
   *                      Defaults to 0 if not specified.
   */
  Cloud(std::vector<float> x, std::vector<float> y, float default_value = 0.f);

  /**
   * @brief Constructs a new Cloud object from lists of `x` and `y` coordinates
   * with assigned values.
   *
   * This constructor allows the creation of a cloud by providing separate lists
   * of `x` and `y` coordinates, along with a list of values associated with
   * each point.
   *
   * @param x A vector of `x` coordinates for the points.
   * @param y A vector of `y` coordinates for the points.
   * @param v A vector of values associated with each point.
   */
  Cloud(std::vector<float> x, std::vector<float> y, std::vector<float> v);

  /**
   * @brief Add a new point to the cloud.
   *
   * This method appends a new point to the list of points in the cloud.
   *
   * @param p The point to be added to the cloud.
   */
  void add_point(const Point &p);

  /**
   * @brief Clear all data from the cloud.
   *
   * This method removes all points from the cloud, leaving it empty.
   */
  void clear();

  /**
   * @brief Get the bounding box of the cloud.
   *
   * This method calculates and returns the axis-aligned bounding box of the
   * cloud. The bounding box is represented as a `Vec4<float>` containing the
   * minimum and maximum coordinates in both the `x` and `y` dimensions.
   *
   * @return Vec4<float> The bounding box of the cloud in the format `[xmin,
   * xmax, ymin, ymax]`.
   */
  Vec4<float> get_bbox();

  /**
   * @brief Calculates the centroid of a set of points.
   *
   * This function computes the center (or centroid) of a collection of points
   * by averaging their coordinates. It sums up all the point coordinates and
   * then divides the result by the total number of points to obtain the average
   * position. The centroid represents the geometric center of the point cloud.
   *
   * @return Point The computed center, represented as a `Point` object, which
   * contains the average (x, y) coordinates of the points.
   */
  Point get_center();

  /**
   * @brief Computes the indices of the points that form the convex hull of a
   * set of points.
   *
   * This function calculates the convex hull of a set of points and returns the
   * indices of these points in the order they appear on the convex hull. The
   * convex hull is the smallest convex polygon that encloses all the given
   * points.
   *
   * @return std::vector<int> A vector containing the indices of the points that
   * make up the convex hull, listed in order.
   *
   * **Example**
   * @include ex_cloud_get_convex_hull.cpp
   *
   * **Result**
   * @image html ex_cloud_get_convex_hull.png
   */
  std::vector<int> get_convex_hull_point_indices();

  /**
   * @brief Get the number of points in the cloud.
   *
   * This method returns the total number of points currently stored in the
   * cloud.
   *
   * @return size_t The number of points in the cloud.
   */
  size_t get_npoints() const;

  /**
   * @brief Get the values assigned to the points in the cloud.
   *
   * This method returns a vector containing the values associated with each
   * point in the cloud.
   *
   * @return std::vector<float> A vector containing the values of all points in
   * the cloud.
   */
  std::vector<float> get_values() const;

  /**
   * @brief Get the maximum value among the points in the cloud.
   *
   * This method returns the maximum value associated with any point in the
   * cloud.
   *
   * @return float The maximum value among the points.
   */
  float get_values_max();

  /**
   * @brief Get the minimum value among the points in the cloud.
   *
   * This method returns the minimum value associated with any point in the
   * cloud.
   *
   * @return float The minimum value among the points.
   */
  float get_values_min();

  /**
   * @brief Get the `x` coordinates of the points in the cloud.
   *
   * This method returns a vector containing the `x` coordinates of all points
   * in the cloud.
   *
   * @return std::vector<float> A vector containing the `x` coordinates of the
   * points.
   */
  virtual std::vector<float> get_x() const;

  /**
   * @brief Get the concatenated `x` and `y` coordinates of the points in the
   * cloud.
   *
   * This method returns a vector containing the `x` and `y` coordinates of all
   * points in the cloud, arranged in the form `[x0, y0, x1, y1, ...]`.
   *
   * @return std::vector<float> A vector containing the concatenated `x` and `y`
   * coordinates of the points.
   */
  virtual std::vector<float> get_xy() const;

  /**
   * @brief Get the `y` coordinates of the points in the cloud.
   *
   * This method returns a vector containing the `y` coordinates of all points
   * in the cloud.
   *
   * @return std::vector<float> A vector containing the `y` coordinates of the
   * points.
   */
  virtual std::vector<float> get_y() const;

  /**
   * @brief Interpolate values from an array at the points' `(x, y)` locations.
   *
   * This method computes interpolated values for each point in the cloud based
   * on its `(x, y)` coordinates and an underlying array, using bilinear
   * interpolation within the specified bounding box.
   *
   * @param array The input array from which to interpolate values.
   * @param bbox The bounding box of the array.
   * @return std::vector<float> A vector containing the interpolated values for
   * each point.
   */
  std::vector<float> interpolate_values_from_array(const Array &array,
                                                   Vec4<float>  bbox);

  /**
   * @brief Print information about the cloud's points.
   *
   * This method prints data related to the cloud's points, including their
   * coordinates and values.
   */
  void print();

  /**
   * @brief Randomize the positions and values of the cloud points.
   *
   * This method randomizes the positions and values of the points in the cloud
   * using a given random seed. The new positions are generated within the
   * specified bounding box.
   *
   * @param seed Random seed number for generating positions and values.
   * @param bbox Bounding box within which the points will be randomized.
   */
  void randomize(uint seed, Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Remap the values of the cloud points to a target range.
   *
   * This method scales the values associated with the cloud points so that they
   * fall within a specified range `[vmin, vmax]`.
   *
   * @param vmin The lower bound of the target range.
   * @param vmax The upper bound of the target range.
   */
  void remap_values(float vmin, float vmax);

  /**
   * @brief Remove a point from the cloud.
   *
   * This method removes a point from the cloud based on its index.
   *
   * @param point_idx Index of the point to be removed.
   */
  void remove_point(int point_idx);

  /**
   * @brief Set new values for the cloud points.
   *
   * This method assigns new values to the cloud points based on a given vector
   * of values. The size of the input vector must match the number of points in
   * the cloud.
   *
   * @param new_values A vector of new values to assign to the points.
   */
  void set_values(std::vector<float> new_values);

  /**
   * @brief Set a single value for all cloud points.
   *
   * This method assigns the same value to all points in the cloud.
   *
   * @param new_value The value to assign to all points.
   */
  void set_values(float new_value);

  /**
   * @brief Set the values of the cloud points using values from an underlying
   * array.
   *
   * This method assigns values to the cloud points by interpolating values from
   * an input array. The positions of the cloud points are mapped to the array
   * using the specified bounding box.
   *
   * @param array The input array from which to derive the values.
   * @param bbox The bounding box that defines the mapping from the cloud
   * points' coordinates to the array's coordinates.
   */
  void set_values_from_array(const Array &array, Vec4<float> bbox);

  /**
   * @brief Set the values of the cloud points based on the distance to the
   * convex hull of the cloud.
   *
   * This method assigns values to the cloud points based on their distance to
   * the convex hull of the cloud. The convex hull must be initialized using
   * `to_graph_delaunay()` before this method is called to ensure the correct
   * indices of the convex hull points are available.
   */
  void set_values_from_chull_distance();

  /**
   * @brief Project the cloud points onto an array.
   *
   * This method projects the cloud points' values onto a given array, mapping
   * their positions within the specified bounding box. The resulting array will
   * be populated with the values of the points at their corresponding
   * positions.
   *
   * @param array The input array where the cloud points' values will be
   * projected.
   * @param bbox The bounding box that defines the mapping from the cloud
   * points' coordinates to the array's coordinates.
   */
  void to_array(Array &array, Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Generate an array filled with the signed distance function (SDF) to
   * the cloud points.
   *
   * This function returns an array where each value represents the signed
   * distance to the nearest cloud point. The distance is positive outside the
   * cloud and negative inside. The result can be domain-warped by applying
   * optional noise arrays to the x and y coordinates.
   *
   * @param shape The shape of the output array (width and height).
   * @param bbox The bounding box that defines the cloud's coordinate system.
   * @param p_noise_x Optional reference to a noise array applied to the
   * x-coordinates for domain warping (not in pixels).
   * @param p_noise_y Optional reference to a noise array applied to the
   * y-coordinates for domain warping (not in pixels).
   * @param bbox_array The bounding box of the destination array.
   * @return Array The resulting array filled with the signed distance function.
   *
   * **Example**
   * @include ex_cloud_sdf.cpp
   *
   * **Result**
   * @image html ex_cloud_sdf.png
   */
  Array to_array_sdf(Vec2<int>   shape,
                     Vec4<float> bbox,
                     Array      *p_noise_x = nullptr,
                     Array      *p_noise_y = nullptr,
                     Vec4<float> bbox_array = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Interpolate the values of an array using the cloud points.
   *
   * This method populates an array with interpolated values based on the
   * positions and values of the cloud points. The interpolation method can be
   * specified, and optional noise arrays can be used for domain warping.
   *
   * @param array The output array that will be populated with interpolated
   * values.
   * @param bbox The bounding box that defines the cloud's coordinate system.
   * @param interpolation_method The method used for interpolation (e.g.,
   * nearest neighbor, bilinear).
   * @param p_noise_x Optional reference to a noise array applied to the
   * x-coordinates for domain warping (not in pixels).
   * @param p_noise_y Optional reference to a noise array applied to the
   * y-coordinates for domain warping (not in pixels).
   * @param bbox_array The bounding box of the destination array.
   *
   * **Example**
   * @include ex_cloud_to_array_interp.cpp
   *
   * **Result**
   * @image html ex_cloud_to_array_interp.png
   */
  void to_array_interp(Array                &array,
                       Vec4<float>           bbox = {0.f, 1.f, 0.f, 1.f},
                       InterpolationMethod2D interpolation_method =
                           InterpolationMethod2D::DELAUNAY,
                       Array      *p_noise_x = nullptr,
                       Array      *p_noise_y = nullptr,
                       Vec4<float> bbox_array = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Export the cloud data to a CSV file.
   *
   * This function saves the cloud points and their associated values to a CSV
   * file.
   *
   * @param fname The name of the output CSV file.
   */
  void to_csv(std::string fname);

  /**
   * @brief Convert the cloud to a graph using Delaunay triangulation.
   *
   * This function generates a graph representation of the cloud using Delaunay
   * triangulation. The resulting graph can be used for various geometric and
   * topological analyses.
   *
   * @return Graph The resulting graph from Delaunay triangulation.
   */
  Graph to_graph_delaunay();

  /**
   * @brief Saves the current data as a PNG image file.
   *
   * This function exports the current data to a PNG file with specified
   * parameters. The image is created using the provided colormap, bounding box,
   * bit depth, and shape. The bounding box determines the area of the data to
   * be included in the image, while the shape defines the dimensions of the
   * output image.
   *
   * @param fname The file name for the output PNG image. This should include
   * the file extension (e.g., "output.png").
   * @param cmap An integer specifying the colormap to be used for rendering the
   * data. This index refers to a predefined colormap.
   * @param bbox A `Vec4<float>` specifying the bounding box of the data to be
   * included in the image. It is given as {xmin, xmax, ymin, ymax}. The default
   * is {0.f, 1.f, 0.f, 1.f}.
   * @param depth An integer specifying the bit depth of the image. It should be
   * a value defined by OpenCV (e.g., `CV_8U` for 8-bit unsigned). The default
   *              is `CV_8U`.
   * @param shape A `Vec2<int>` specifying the dimensions of the output image.
   * It is given as {width, height}. The default is {512, 512}.
   */
  void to_png(const std::string &fname,
              int                cmap,
              Vec4<float>        bbox = {0.f, 1.f, 0.f, 1.f},
              int                depth = CV_8U,
              Vec2<int>          shape = {512, 512});
};

/**
 * @brief Merges two point clouds into one.
 *
 * This function combines two separate point clouds into a single cloud by
 * appending the points from the second cloud to the first. The resulting cloud
 * contains all points from both input clouds.
 *
 * @param cloud1 The first point cloud to be merged. This cloud will be the base
 * cloud to which the points from the second cloud are added.
 * @param cloud2 The second point cloud whose points will be appended to the
 * first cloud.
 *
 * @return Cloud The resulting point cloud that includes all points from both
 *                `cloud1` and `cloud2`.
 */
Cloud merge_cloud(const Cloud &cloud1, const Cloud &cloud2);

} // namespace hmap