/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file path.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
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
#include "highmap/geometry/cloud.hpp"

namespace hmap
{

/**
 * @class Path
 * @brief Represents an ordered set of points in 2D, forming a polyline (open or
 * closed).
 *
 * The `Path` class extends the `Cloud` class to represent a sequence of points
 * in 2D space that are connected in a specific order, forming a polyline. The
 * polyline can be either open or closed, depending on whether the first and
 * last points are connected. This class provides methods for various geometric
 * operations, including path length calculation, point insertion, and more.
 *
 * **Example**
 * @include ex_path.cpp
 *
 * **Result**
 * @image html ex_path.png
 */
class Path : public Cloud
{
public:
  /**
   * @brief Defines wether the path is close or open.
   */
  bool closed;

  /**
   * @brief Construct a new Path object.
   * @param closed Open/close path.
   */
  Path(bool closed = false) : Cloud(), closed(closed){};

  /**
   * @brief Construct a new Path object with random positions and values.
   * @param npoints Number of points.
   * @param seed Random seed number.
   * @param bbox Bounding box.
   * @param closed Open/close path.
   */
  Path(int         npoints,
       uint        seed,
       Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f},
       bool        closed = false)
      : Cloud(npoints, seed, bbox), closed(closed){};

  /**
   * @brief Construct a new Path object based on a list of points.
   * @param points Points of the cloud.
   * @param closed Open/close path.
   */
  Path(std::vector<Point> points, bool closed = false)
      : Cloud(points), closed(closed){};

  /**
   * @brief Construct a new Cloud object based on a list of `x` and `y`
   * coordinates.
   * @param x Point `x` coordinates.
   * @param y Point `y` coordinates.
   * @param closed Open/close path.
   */
  Path(std::vector<float> x, std::vector<float> y, bool closed = false)
      : Cloud(x, y), closed(closed){};

  /**
   * @brief Construct a new Cloud object based on a list of `x` and `y`
   * coordinates and a list of values.
   * @param x Point `x` coordinates.
   * @param y Point `y` coordinates.
   * @param v Assigned values.
   * @param closed Open/close path.
   */
  Path(std::vector<float> x,
       std::vector<float> y,
       std::vector<float> v,
       bool               closed = false)
      : Cloud(x, y, v), closed(closed){};

  /**
   * @brief "Smooth" the path using Bezier curves.
   * @param curvature_ratio Amount of curvature (usually in [-1, 1] and commonly
   * > 0).
   * @param edge_divisions Edge sub-divisions of each edge.
   * **Example**
   * @include ex_path_bezier.cpp
   * **Result**
   * @image html ex_path_bezier.png
   */
  void bezier(float curvature_ratio = 0.3f, int edge_divisions = 10);

  /**
   * @brief "Smooth" the path using Bezier curves (alternative).
   * @param curvature_ratio Amount of curvature (usually in [-1, 1] and commonly
   * > 0).
   * @param edge_divisions Edge sub-divisions of each edge.
   * **Example**
   * @include ex_path_bezier_round.cpp
   * **Result**
   * @image html ex_path_bezier_round.png
   */
  void bezier_round(float curvature_ratio = 0.3f, int edge_divisions = 10);

  /**
   * @brief "Smooth" the path using BSpline curves.
   * @param edge_divisions Edge sub-divisions of each edge.
   * **Example**
   * @include ex_path_bspline.cpp
   * **Result**
   * @image html ex_path_bspline.png
   */
  void bspline(int edge_divisions = 10);

  /**
   * @brief "Smooth" the path using CatmullRom curves.
   * @param edge_divisions Edge sub-divisions of each edge.
   * **Example**
   * @include ex_path_catmullrom.cpp
   * **Result**
   * @image html ex_path_catmullrom.png
   */
  void catmullrom(int edge_divisions = 10);

  /**
   * @brief Clear the path data.
   */
  void clear();

  /**
   * @brief Divide path by adding point based on the lowest elevation difference
   * path between each ends of the egdes.
   * @param array Elevation map.
   * @param bbox Domain bounding box.
   * @param edge_divisions Edge sub-divisions (set to 0 for a division based on
   * the array shape).
   * @param elevation_ratio Balance between absolute elevation and elevation
   * difference in the cost function.
   * @param distance_exponent Distance exponent of the dijkstra weight function.
   * @param p_mask_nogo Reference to the mask defining areas to avoid.
   * **Example**
   * @include ex_path_dijkstra.cpp
   * **Result**
   * @image html ex_path_dijkstra.png
   * @see {@link Array::find_path_dijkstra}
   */
  void dijkstra(Array      &array,
                Vec4<float> bbox,
                int         edge_divisions = 0,
                float       elevation_ratio = 0.f,
                float       distance_exponent = 0.5f,
                Array      *p_mask_nogo = nullptr);

  /**
   * @brief Divide path by adding a point in-between each pair of consecutive
   * points.
   */
  void divide();

  /**
   * @brief "Fractalize" the path by adding points and shuffling their
   * positions.
   * @param iterations Number of iterations.
   * @param seed Random seed number.
   * @param sigma Half-width of the random Gaussian displacement, normalized by
   * the distance between points.
   * @param orientation Displacement orientation (0 for random inward/outward
   * displacement, 1 to inflate the path and -1 to deflate the path).
   * @param persistence Noise persistence (with iteration number).
   * @param p_control_field Reference to the array locally controlling the
   * amplitude of the added displacement.
   * **Example**
   * @include ex_path_fractalize.cpp
   * **Result**
   * @image html ex_path_fractalize.png
   */
  void fractalize(int   iterations,
                  uint  seed,
                  float sigma = 0.3f,
                  int   orientation = 0,
                  float persistence = 1.f);

  void fractalize(int         iterations,
                  uint        seed,
                  Array      &control_field,
                  Vec4<float> bbox,
                  float       sigma = 0.3f,
                  int         orientation = 0,
                  float       persistence = 1.f); ///< @overload

  /**
   * @brief Get the arc length of the path (i.e. cumulative distance normalized
   * in [0, 1]).
   * @return std::vector<float>
   */
  std::vector<float> get_arc_length();

  /**
   * @brief Get the cumulative distance of the path (defined at each points).
   * @return std::vector<float> Cumulative distance.
   */
  std::vector<float> get_cumulative_distance();

  /**
   * @brief Get the values assigned of the points.
   * @return std::vector<float> Values
   */
  std::vector<float> get_values() const
  {
    std::vector<float> v(this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
      v[i] = this->points[i].v;

    if (this->closed && this->get_npoints() > 0) v.push_back(this->points[0].v);
    return v;
  }

  /**
   * @brief Get the `x` of the points.
   * @return std::vector<float> `x` values.
   */
  std::vector<float> get_x() const
  {
    std::vector<float> x(this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
      x[i] = this->points[i].x;

    if (this->closed && this->get_npoints() > 0) x.push_back(this->points[0].x);
    return x;
  }

  /** Get the points coordinates of a single vector (x0, y0, x1, y1,...).
   * @brief Get the xy object
   * @return std::vector<float> Coordinates.
   */
  std::vector<float> get_xy() const
  {
    std::vector<float> xy(2 * this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
    {
      xy[2 * i] = this->points[i].x;
      xy[2 * i + 1] = this->points[i].y;
    }

    if (this->closed && this->get_npoints() > 0)
    {
      xy.push_back(this->points[0].x);
      xy.push_back(this->points[0].y);
    }
    return xy;
  }

  /**
   * @brief Get the `y` of the points.
   * @return std::vector<float> `y` values.
   */
  std::vector<float> get_y() const
  {
    std::vector<float> y(this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
      y[i] = this->points[i].y;

    if (this->closed && this->get_npoints() > 0) y.push_back(this->points[0].y);
    return y;
  }

  /**
   * @brief Add "meanders" to the path.
   * @param ratio Meander amplitude ratio.
   * @param noise_ratio Randomness ratio.
   * @param seed Random seed number.
   * @param iterations Number of iterations.
   * @param edge_divisions Edge sub-divisions of each edge.
   * **Example**
   * @include ex_path_meanderize.cpp
   * **Result**
   * @image html ex_path_meanderize.png
   */
  void meanderize(float ratio,
                  float noise_ratio = 0.1f,
                  uint  seed = 1,
                  int   iterations = 1,
                  int   edge_divisions = 10);

  /**
   * @brief Reorder points using a nearest neighbor search.
   * @param start_index Starting point of the search.
   */
  void reorder_nns(int start_index = 0);

  /**
   * @brief Resample the path based to get (approximately) a `delta` distance
   * between points.
   * @param delta Target distance between the points.
   */
  void resample(float delta);

  /**
   * @brief Resample the path in order to get fairly uniform distance between
   * each consecutive points.
   */
  void resample_uniform();

  /**
   * @brief Reverse point order.
   */
  void reverse();

  /**
   * @brief Return the value of the angle of the closest edge to the point (x,
   * y), assuming a closed path.
   * @param x x coordinate.
   * @param y y coordinate.
   * @return float Edge angle (radians).
   *  * **Example**
   * @include ex_path_sdf.cpp
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_angle_closed(float x, float y);

  /**
   * @brief Return the value of the angle of the closest edge to the point (x,
   * y), assuming an open path.
   * @param x x coordinate.
   * @param y y coordinate.
   * @return float Edge angle (radians).
   *  * **Example**
   * @include ex_path_sdf.cpp
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_angle_open(float x, float y);

  /**
   * @brief Return the value of the signed distance function at (x, y), assuming
   * a closed path.
   * @param x x coordinate.
   * @param y y coordinate.
   * @return float Signed distance.
   * **Example**
   * @include ex_path_sdf.cpp
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_closed(float x, float y);

  /**
   * @brief Return the value of the elevation at (x, y) away from the path based
   * on a downslope `slope`, assuming a closed path.
   * @param x x coordinate.
   * @param y y coordinate.
   * @param slope Downslope.
   * @return float Signed distance.
   *  * **Example**
   * @include ex_path_sdf.cpp
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_elevation_closed(float x, float y, float slope);

  /**
   * @brief Return the value of the elevation at (x, y) away from the path based
   * on a downslope `slope`, assuming an open path.
   * @param x x coordinate.
   * @param y y coordinate.
   * @param slope Downslope.
   * @return float Signed distance.
   *  * **Example**
   * @include ex_path_sdf.cpp
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_elevation_open(float x, float y, float slope);

  /**
   * @brief Return the value of the signed distance function at (x, y), assuming
   * an open path.
   * @param x x coordinate.
   * @param y y coordinate.
   * @return float Signed distance.
   *  * **Example**
   * @include ex_path_sdf.cpp
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_open(float x, float y);

  /**
   * @brief Subsample the path by keeping only every n-th point.
   * @param step Keep every 'step' points.
   */
  void subsample(int step);

  /**
   * @brief Project path points to an array.
   * @param array Input array.
   * @param bbox Bounding box of the array.
   * @param filled Activate flood filling of the contour.
   */
  void to_array(Array &array, Vec4<float> bbox, bool filled = false);

  /**
   * @brief Return an array filled with the signed distance function to the path
   * @param shape Output array shape.
   * @param bbox Bounding box.
   * @param p_noise_x, p_noise_y Reference to the input noise arrays.
   * @param bbox_array Bounding box of the destination array.
   * @return Array Resulting array.
   *  * **Example**
   * @include ex_path_sdf.cpp
   * **Result**
   * @image html ex_path_sdf.png
   */
  Array to_array_sdf(Vec2<int>   shape,
                     Vec4<float> bbox,
                     Array      *p_noise_x = nullptr,
                     Array      *p_noise_y = nullptr,
                     Vec4<float> bbox_array = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Export path as png image file.
   * @param fname File name.
   * @param shape Image resolution.
   */
  void to_png(std::string fname, Vec2<int> shape = {512, 512});
};

/**
 * @brief Dig a path on a heightmap.
 * @param z Input array.
 * @param path Path to dig.
 * @param width Path width radius (in pixels).
 * @param decay Path border decay radius (in pixels).
 * @param flattening_radius Path elevation flattening radius (in pixels).
 * @param force_downhill Force the path elevation to be monotically decreasing.
 * @param bbox  Bounding box.
 *
 * **Example**
 * @include ex_dig_path.cpp
 *
 * **Result**
 * @image html ex_dig_path.png
 */
void dig_path(Array      &z,
              Path       &path,
              int         width = 1,
              int         decay = 2,
              int         flattening_radius = 16,
              bool        force_downhill = false,
              Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f},
              float       depth = 0.f);

} // namespace hmap