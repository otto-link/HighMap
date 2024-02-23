/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file geometry.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <random>
#include <vector>

#include "highmap/array.hpp"
#include "highmap/vector.hpp"

namespace hmap
{

class Graph;

/**
 * @brief Point class, to manipulate points in 2D.
 *
 */
class Point
{
public:
  /**
   * @brief Point `x` coordinate.
   *
   */
  float x;

  /**
   * @brief Point `y` coordinate.
   *
   */
  float y;

  /**
   * @brief Value assigned to the point.
   *
   */
  float v;

  /**
   * @brief Construct a new Point object
   *
   * @param x Point `x` coordinate.
   * @param y Point `y` coordinate.
   * @param v Assigned value.
   */
  Point(float x, float y, float v = 0.f) : x(x), y(y), v(v)
  {
  }

  Point() : x(0.f), y(0.f), v(0.f)
  {
  } ///< @overload
};

/**
 * @brief Cloud class, to manipulate unordered set of points in 2D.
 *
 */
class Cloud
{
public:
  /**
   * @brief Points of the cloud.
   *
   */
  std::vector<Point> points = {};

  /**
   * @brief Indices of points on the convex hull, counter-clockwise.
   *
   */
  std::vector<int> convex_hull = {};

  /**
   * @brief Construct a new Cloud object.
   *
   */
  Cloud(){};

  /**
   * @brief Construct a new Cloud object with random positions and values.
   *
   * @param npoints Number of points.
   * @param seed Random seed number.
   * @param bbox Bounding box.
   */
  Cloud(int npoints, uint seed, Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Construct a new Cloud object based on a list of points.
   *
   * @param points Points of the cloud.
   */
  Cloud(std::vector<Point> points) : points(points){};

  /**
   * @brief Construct a new Cloud object based on a list of `x` and `y`
   * coordinates.
   *
   * @param x Point `x` coordinates.
   * @param y Point `y` coordinates.
   * @param default_value Default value associated to the point.
   */
  Cloud(std::vector<float> x, std::vector<float> y, float default_value = 0.f)
  {
    for (size_t k = 0; k < x.size(); k++)
    {
      Point p(x[k], y[k], default_value);
      this->add_point(p);
    }
  };

  /**
   * @brief Construct a new Cloud object based on a list of `x` and `y`
   * coordinates and a list of values.
   *
   * @param x Point `x` coordinates.
   * @param y Point `y` coordinates.
   * @param v Assigned values.
   */
  Cloud(std::vector<float> x, std::vector<float> y, std::vector<float> v)
  {
    for (size_t k = 0; k < x.size(); k++)
    {
      Point p(x[k], y[k], v[k]);
      this->add_point(p);
    }
  };

  //----------------------------------------
  // accessors
  //----------------------------------------

  /**
   * @brief Get the bounding box of the cloud.
   *
   * @return std::vector<float> Bounding box `[xmin, xmax, ymin, ymax]`.
   */
  Vec4<float> get_bbox()
  {
    std::vector<float> x = this->get_x();
    std::vector<float> y = this->get_y();
    Vec4<float>        bbox;
    {
      float xmin = *std::min_element(x.begin(), x.end());
      float xmax = *std::max_element(x.begin(), x.end());
      float ymin = *std::min_element(y.begin(), y.end());
      float ymax = *std::max_element(y.begin(), y.end());
      bbox = {xmin, xmax, ymin, ymax};
    }
    return bbox;
  }

  /**
   * @brief Get the center cloud.
   *
   * @return Vec2<float> Center (x, y) coordinates.
   */
  Vec2<float> get_center()
  {
    Vec2<float> xy = Vec2<float>(0.f, 0.f);

    for (auto &p : this->points)
    {
      xy.x += p.x;
      xy.y += p.y;
    }

    xy.x /= (float)this->points.size();
    xy.y /= (float)this->points.size();

    return xy;
  }

  /**
   * @brief Get the number of points.
   *
   * @return size_t Number of points.
   */
  size_t get_npoints() const
  {
    return this->points.size();
  }

  /**
   * @brief Get the values assigned of the points.
   *
   * @return std::vector<float> Values
   */
  std::vector<float> get_values() const
  {
    std::vector<float> v(this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
      v[i] = this->points[i].v;
    return v;
  }

  /**
   * @brief Get the maximum value.
   *
   * @return float Maximum value
   */
  float get_values_max();

  /**
   * @brief Get the minimum value.
   *
   * @return float Minimum value
   */
  float get_values_min();

  /**
   * @brief Get the `x` of the points.
   *
   * @return std::vector<float> `x` values.
   */
  virtual std::vector<float> get_x() const
  {
    std::vector<float> x(this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
      x[i] = this->points[i].x;
    return x;
  }

  /** Get the points coordinates of a single vector (x0, y0, x1, y1,...).
   * @brief Get the xy object
   *
   * @return std::vector<float> Coordinates.
   */
  virtual std::vector<float> get_xy() const
  {
    std::vector<float> xy(2 * this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
    {
      xy[2 * i] = this->points[i].x;
      xy[2 * i + 1] = this->points[i].y;
    }
    return xy;
  }

  /**
   * @brief Get the `y` of the points.
   *
   * @return std::vector<float> `y` values.
   */
  virtual std::vector<float> get_y() const
  {
    std::vector<float> y(this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
      y[i] = this->points[i].y;
    return y;
  }

  /**
   * @brief Set the values assigned to the points.
   *
   * @param new_values New values.
   */
  void set_values(std::vector<float> new_values)
  {
    for (size_t k = 0; k < this->get_npoints(); k++)
      this->points[k].v = new_values[k];
  }

  void set_values(float new_value)
  {
    for (auto &p : this->points)
      p.v = new_value;
  } ///< @overload

  /**
   * @brief Set the values using values of an underlying array.
   *
   * @param array Input array.
   * @param bbox Array bounding box.
   */
  void set_values_from_array(const Array &array, Vec4<float> bbox);

  /**
   * @brief Set the values based on the distance to convex hull of the
   * cloud (requires a call to `to_graph_delaunay()` before to
   * initialize convex hull point indices.
   */
  void set_values_from_chull_distance();

  //----------------------------------------
  // methods
  //----------------------------------------

  /**
   * @brief Add a point to the cloud.
   *
   * @param p New point.
   */
  void add_point(const Point &p)
  {
    this->points.push_back(p);
  }

  /**
   * @brief Clear the cloud data.
   */
  void clear();

  /**
   * @brief Get the values from an underlying array at location `(x, y)`.
   *
   * @param array Input array.
   * @param bbox Array bounding box.
   * @return std::vector<float> Values.
   */
  std::vector<float> interpolate_values_from_array(const Array &array,
                                                   Vec4<float>  bbox);

  /**
   * @brief Print some data.
   *
   */
  void print();

  /**
   * @brief Randomize positions and values of the cloud points.
   *
   * @param seed Random seed number.
   * @param bbox Bounding box.
   */
  void randomize(uint seed, Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Remove a point from the cloud.
   *
   * @param point_idx Point index.
   */
  void remove_point(int point_idx)
  {
    this->points.erase(this->points.begin() + point_idx);
  }

  /**
   * @brief Project cloud points to an array.
   *
   * @param array Input array.
   * @param bbox Bounding box of the array.
   */
  void to_array(Array &array, Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Return an array filled with the signed distance function to the
   * cloud
   *
   * @param shape Output array shape.
   * @param bbox Bounding box.
   * @param p_noise_x, p_noise_y Reference to the input noise array used for
   * domain warping (NOT in pixels, with respect to a unit domain).
   * @param shift Noise shift {xs, ys} for each directions, with respect to a
   * unit domain.
   * @param scale Domain scaling, in [0, 1].
   * @return Array Resulting array.
   *
   *  * **Example**
   * @include ex_cloud_sdf.cpp
   *
   * **Result**
   * @image html ex_cloud_sdf.png
   */
  Array to_array_sdf(Vec2<int>   shape,
                     Vec4<float> bbox,
                     Array      *p_noise_x = nullptr,
                     Array      *p_noise_y = nullptr,
                     Vec2<float> shift = {0.f, 0.f},
                     Vec2<float> scale = {1.f, 1.f});

  /**
   * @brief Interpolate array value using cloud points.
   *
   * @param array Input array.
   * @param bbox Bounding box of the array.
   *
   * **Example**
   * @include ex_cloud_to_array_interp.cpp
   *
   * **Result**
   * @image html ex_cloud_to_array_interp.png
   */
  void to_array_interp(Array      &array,
                       Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f},
                       int         interpolation_method = 0,
                       Array      *p_noise_x = nullptr,
                       Array      *p_noise_y = nullptr,
                       Vec2<float> shift = {0.f, 0.f},
                       Vec2<float> scale = {1.f, 1.f});

  /**
   * @brief Export data to a csv file.
   *
   * @param fname File name.
   */
  void to_csv(std::string fname);

  /**
   * @brief Convert a cloud to a graph using Delaunay triangulation.
   *
   * @return Graph
   */
  Graph to_graph_delaunay();
};

/**
 * @brief Graph class, to manipulate graph in 2D.
 *
 * **Example**
 * @include ex_graph.cpp
 *
 * **Result**
 * @image html ex_graph0.png
 */
class Graph : public Cloud
{
public:
  /**
   * @brief Edges of the graph.
   *
   */
  std::vector<std::vector<int>> edges = {};

  /**
   * @brief Edge weights.
   *
   */
  std::vector<float> weights = {};

  /**
   * @brief Store point connectivity.
   *
   */
  std::vector<std::vector<int>> connectivity = {};

  /**
   * @brief Adjacency matrix.
   *
   */
  std::map<std::pair<int, int>, float> adjacency_matrix;

  /**
   * @brief Construct a new Graph object.
   *
   */
  Graph() : Cloud(){};

  /**
   * @brief Construct a new Graph object based on a cloud of points.
   *
   * @param cloud
   */
  Graph(Cloud cloud) : Cloud(cloud){};

  Graph(std::vector<Point> points) : Cloud(points){};

  Graph(std::vector<float> x, std::vector<float> y) : Cloud(x, y){};

  /**
   * @brief Get the length of edge `k`.
   *
   * @param k Edge index.
   * @return float Euclidian length.
   */
  float get_edge_length(int k);

  /**
   * @brief Return x coordinates of the edges (as pairs)
   *
   * @return std::vector<float> Coordinates.
   */
  std::vector<float> get_edge_x_pairs();

  /**
   * @brief Return y coordinates of the edges (as pairs)
   *
   * @return std::vector<float> Coordinates.
   */
  std::vector<float> get_edge_y_pairs();

  /**
   * @brief Get the length of all the edge lengths.
   *
   * @return std::vector<float>
   */
  std::vector<float> get_lengths();

  /**
   * @brief Get the number of edges.
   *
   * @return size_t
   */
  size_t get_nedges()
  {
    return this->edges.size();
  }

  /**
   * @brief Add an edge.
   *
   * @param edge Edge indices {point #1, point #2}.
   * @param weight Edge weight (if not provided, the Euclidian distance between
   * the edge point is used as a default weight);
   */
  void add_edge(std::vector<int> edge, float weight)
  {
    this->edges.push_back(edge);
    this->weights.push_back(weight);
  }

  void add_edge(std::vector<int> edge)
  {
    this->edges.push_back(edge);
    this->weights.push_back(this->get_edge_length(this->get_nedges() - 1));
  } ///< @overload

  /**
   * @brief Return the route between tow points of the graph using the
   * Dijkstra's algorithm.
   *
   * @param source_point_index Starting point index.
   * @param target_point_index Ending point index.
   * @return std::vector<int> List of point indices.
   *
   *  * **Example**
   * @include ex_graph_dijkstra.cpp
   */
  std::vector<int> dijkstra(int source_point_index, int target_point_index);

  /**
   * @brief Return a "minimum spanning tree" graph using Prim's algorithm.
   *
   * @return Graph MST graph.
   *
   * **Example**
   * @include ex_graph_minimum_spanning_tree_prim.cpp
   *
   * **Result**
   * @image html ex_graph_minimum_spanning_tree_prim0.png
   * @image html ex_graph_minimum_spanning_tree_prim1.png
   */
  Graph minimum_spanning_tree_prim();

  /**
   * @brief Print some data.
   *
   */
  void print();

  /**
   * @brief Remove orphan points from the graph.
   *
   * Orphan points are points not connected to any edge.
   *
   * @return Graph New graph without orphan points.
   */
  Graph remove_orphan_points();

  /**
   * @brief Project graph to an array.
   *
   * @param array Input array.
   * @param bbox Bounding box of the array.
   * @param color_by_edge_weight Color by edge weight or by node values else.
   */
  void to_array(Array      &array,
                Vec4<float> bbox,
                bool        color_by_edge_weight = true);

  /**
   * @brief Fractalize graph edge and project to an array.
   *
   * @param array Input array.
   * @param bbox Bounding box of the array.
   * @param iterations Number of iterations.
   * @param seed Random seed number.
   * @param sigma Half-width of the random Gaussian displacement, normalized by
   * the distance between points.
   * @param orientation Displacement orientation (0 for random inward/outward
   * displacement, 1 to inflate the path and -1 to deflate the path).
   * @param persistence Noise persistence (with iteration number).
   */
  void to_array_fractalize(Array      &array,
                           Vec4<float> bbox,
                           int         iterations,
                           uint        seed,
                           float       sigma = 0.3f,
                           int         orientation = 0.f,
                           float       persistence = 1.f);

  /**
   * @brief Return an array filled with the signed distance function to the
   * graph
   *
   * @param shape Output array shape.
   * @param bbox Bounding box.
   * @param p_noise_x, p_noise_y Reference to the input noise array used for
   * domain warping (NOT in pixels, with respect to a unit domain).
   * @param shift Noise shift {xs, ys} for each directions, with respect to a
   * unit domain.
   * @param scale Domain scaling, in [0, 1].
   * @return Array Resulting array.
   *
   *  * **Example**
   * @include ex_cloud_sdf.cpp
   *
   * **Result**
   * @image html ex_cloud_sdf.png
   */
  Array to_array_sdf(Vec2<int>   shape,
                     Vec4<float> bbox,
                     Array      *p_noise_x = nullptr,
                     Array      *p_noise_y = nullptr,
                     Vec2<float> shift = {0.f, 0.f},
                     Vec2<float> scale = {1.f, 1.f});

  /**
   * @brief Export graph to csv files.
   *
   * @param fname_xy Filename for the node `(x, y)` coordinates.
   * @param fname_adjacency Filename for the adjacency matrix.
   */
  void to_csv(std::string fname_xy, std::string fname_adjacency);

  /**
   * @brief Export graph as png image file.
   *
   * @param fname File name.
   * @param shape Image resolution.
   */
  void to_png(std::string fname, Vec2<int> shape = {512, 512});

  /**
   * @brief Update the adjacency matrix.
   *
   */
  void update_adjacency_matrix();

  /**
   * @brief Update point connectivity.
   *
   */
  void update_connectivity();
};

/**
 * @brief Path class, to manipulate ordered set of points in 2D.
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
   *
   */
  bool closed;

  /**
   * @brief Construct a new Path object.
   *
   * @param closed Open/close path.
   */
  Path(bool closed = false) : Cloud(), closed(closed){};

  /**
   * @brief Construct a new Path object with random positions and values.
   *
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
   *
   * @param points Points of the cloud.
   * @param closed Open/close path.
   */
  Path(std::vector<Point> points, bool closed = false)
      : Cloud(points), closed(closed){};

  /**
   * @brief Construct a new Cloud object based on a list of `x` and `y`
   * coordinates.
   *
   * @param x Point `x` coordinates.
   * @param y Point `y` coordinates.
   * @param closed Open/close path.
   */
  Path(std::vector<float> x, std::vector<float> y, bool closed = false)
      : Cloud(x, y), closed(closed){};

  /**
   * @brief Construct a new Cloud object based on a list of `x` and `y`
   * coordinates and a list of values.
   *
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

  //----------------------------------------
  // getters / setters
  //----------------------------------------

  /**
   * @brief Get the arc length of the path (i.e. cumulative distance normalized
   * in [0, 1]).
   *
   * @return std::vector<float>
   */
  std::vector<float> get_arc_length();

  /**
   * @brief Get the cumulative distance of the path (defined at each points).
   *
   * @return std::vector<float> Cumulative distance.
   */
  std::vector<float> get_cumulative_distance();

  /**
   * @brief Get the values assigned of the points.
   *
   * @return std::vector<float> Values
   */
  std::vector<float> get_values() const
  {
    std::vector<float> v(this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
      v[i] = this->points[i].v;

    if (this->closed && this->get_npoints() > 0)
      v.push_back(this->points[0].v);
    return v;
  }

  /**
   * @brief Get the `x` of the points.
   *
   * @return std::vector<float> `x` values.
   */
  std::vector<float> get_x() const
  {
    std::vector<float> x(this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
      x[i] = this->points[i].x;

    if (this->closed && this->get_npoints() > 0)
      x.push_back(this->points[0].x);
    return x;
  }

  /** Get the points coordinates of a single vector (x0, y0, x1, y1,...).
   * @brief Get the xy object
   *
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
   *
   * @return std::vector<float> `y` values.
   */
  std::vector<float> get_y() const
  {
    std::vector<float> y(this->get_npoints());
    for (size_t i = 0; i < this->get_npoints(); i++)
      y[i] = this->points[i].y;

    if (this->closed && this->get_npoints() > 0)
      y.push_back(this->points[0].y);
    return y;
  }

  //----------------------------------------
  // methods
  //----------------------------------------

  /**
   * @brief "Smooth" the path using Bezier curves.
   *
   * @param curvature_ratio Amount of curvature (usually in [-1, 1] and commonly
   * > 0).
   * @param edge_divisions Edge sub-divisions of each edge.
   *
   * **Example**
   * @include ex_path_bezier.cpp
   *
   * **Result**
   * @image html ex_path_bezier.png
   */
  void bezier(float curvature_ratio = 0.3f, int edge_divisions = 10);

  /**
   * @brief "Smooth" the path using Bezier curves (alternative).
   *
   * @param curvature_ratio Amount of curvature (usually in [-1, 1] and commonly
   * > 0).
   * @param edge_divisions Edge sub-divisions of each edge.
   *
   * **Example**
   * @include ex_path_bezier_round.cpp
   *
   * **Result**
   * @image html ex_path_bezier_round.png
   */
  void bezier_round(float curvature_ratio = 0.3f, int edge_divisions = 10);

  /**
   * @brief "Smooth" the path using BSpline curves.
   *
   * @param edge_divisions Edge sub-divisions of each edge.
   *
   *
   * **Example**
   * @include ex_path_bspline.cpp
   *
   * **Result**
   * @image html ex_path_bspline.png
   */
  void bspline(int edge_divisions = 10);

  /**
   * @brief "Smooth" the path using CatmullRom curves.
   *
   * @param edge_divisions Edge sub-divisions of each edge.
   *
   *
   * **Example**
   * @include ex_path_catmullrom.cpp
   *
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
   *
   * @param array Elevation map.
   * @param bbox Domain bounding box.
   * @param edge_divisions Edge sub-divisions (set to 0 for a division based on
   * the array shape).
   * @param elevation_ratio Balance between absolute elevation and elevation
   * difference in the cost function.
   * @param distance_exponent Distance exponent of the dijkstra weight function.
   * @param p_mask_nogo Reference to the mask defining areas to avoid.
   *
   * **Example**
   * @include ex_path_dijkstra.cpp
   *
   * **Result**
   * @image html ex_path_dijkstra.png
   *
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
   *
   */
  void divide();

  /**
   * @brief "Fractalize" the path by adding points and shuffling their
   * positions.
   *
   * @param iterations Number of iterations.
   * @param seed Random seed number.
   * @param sigma Half-width of the random Gaussian displacement, normalized by
   * the distance between points.
   * @param orientation Displacement orientation (0 for random inward/outward
   * displacement, 1 to inflate the path and -1 to deflate the path).
   * @param persistence Noise persistence (with iteration number).
   */
  void fractalize(int   iterations,
                  uint  seed,
                  float sigma = 0.3f,
                  int   orientation = 0,
                  float persistence = 1.f);

  /**
   * @brief Add "meanders" to the path.
   *
   * @param ratio Meander amplitude ratio.
   * @param noise_ratio Randomness ratio.
   * @param seed Random seed number.
   * @param iterations Number of iterations.
   * @param edge_divisions Edge sub-divisions of each edge.
   *
   * **Example**
   * @include ex_path_meanderize.cpp
   *
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
   *
   * @param start_index Starting point of the search.
   */
  void reorder_nns(int start_index = 0);

  /**
   * @brief Resample the path based to get (approximately) a `delta` distance
   * between points.
   *
   * @param delta Target distance between the points.
   */
  void resample(float delta);

  /**
   * @brief Resample the path in order to get fairly uniform distance between
   * each consecutive points.
   *
   */
  void resample_uniform();

  /**
   * @brief Reverse point order.
   */
  void reverse();

  /**
   * @brief Return the value of the angle of the closest edge to the point (x,
   * y), assuming a closed path.
   *
   * @param x x coordinate.
   * @param y y coordinate.
   * @return float Edge angle (radians).
   *
   *  * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_angle_closed(float x, float y);

  /**
   * @brief Return the value of the angle of the closest edge to the point (x,
   * y), assuming an open path.
   *
   * @param x x coordinate.
   * @param y y coordinate.
   * @return float Edge angle (radians).
   *
   *  * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_angle_open(float x, float y);

  /**
   * @brief Return the value of the signed distance function at (x, y), assuming
   * a closed path.
   *
   * @param x x coordinate.
   * @param y y coordinate.
   * @return float Signed distance.
   *
   * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_closed(float x, float y);

  /**
   * @brief Return the value of the elevation at (x, y) away from the path based
   * on a downslope `slope`, assuming a closed path.
   *
   * @param x x coordinate.
   * @param y y coordinate.
   * @param slope Downslope.
   * @return float Signed distance.
   *
   *  * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_elevation_closed(float x, float y, float slope);

  /**
   * @brief Return the value of the elevation at (x, y) away from the path based
   * on a downslope `slope`, assuming an open path.
   *
   * @param x x coordinate.
   * @param y y coordinate.
   * @param slope Downslope.
   * @return float Signed distance.
   *
   *  * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_elevation_open(float x, float y, float slope);

  /**
   * @brief Return the value of the signed distance function at (x, y), assuming
   * an open path.
   *
   * @param x x coordinate.
   * @param y y coordinate.
   * @return float Signed distance.
   *
   *  * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   */
  float sdf_open(float x, float y);

  /**
   * @brief Subsample the path by keeping only every n-th point.
   *
   * @param step Keep every 'step' points.
   */
  void subsample(int step);

  /**
   * @brief Project path points to an array.
   *
   * @param array Input array.
   * @param bbox Bounding box of the array.
   * @param filled Activate flood filling of the contour.
   */
  void to_array(Array &array, Vec4<float> bbox, bool filled = false);

  /**
   * @brief Return an array filled with the signed distance function to the path
   *
   * @param shape Output array shape.
   * @param bbox Bounding box.
   * @param p_noise_x, p_noise_y Reference to the input noise array used for
   * domain warping (NOT in pixels, with respect to a unit domain).
   * @param shift Noise shift {xs, ys} for each directions, with respect to a
   * unit domain.
   * @param scale Domain scaling, in [0, 1].
   * @return Array Resulting array.
   *
   *  * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf.png
   */
  Array to_array_sdf(Vec2<int>   shape,
                     Vec4<float> bbox,
                     Array      *p_noise_x = nullptr,
                     Array      *p_noise_y = nullptr,
                     Vec2<float> shift = {0.f, 0.f},
                     Vec2<float> scale = {1.f, 1.f});

  /**
   * @brief Export path as png image file.
   *
   * @param fname File name.
   * @param shape Image resolution.
   */
  void to_png(std::string fname, Vec2<int> shape = {512, 512});
};

//----------------------------------------
// Path functions
//----------------------------------------

/**
 * @brief Dig a path on a heightmap.
 *
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

//----------------------------------------
// Point functions
//----------------------------------------

/**
 * @brief Return the polar angle between two points.
 *
 * @param p1 1st point.
 * @param p2 2nd point.
 * @return float Angle in radians.
 */
float angle(const Point &p1, const Point &p2);

/**
 * @brief Return the distance between two points in 2D, using only `x` and `y`
 * coordinates.
 *
 * @param p1 1st point.
 * @param p2 2nd point.
 * @return float Euclidian distance.
 */
float distance(const Point &p1, const Point &p2);

/**
 * @brief Return the linear interpolation between two points by a parameter `t`.
 *
 * @param p1 1st point.
 * @param p2 2nd point.
 * @param t Interpolation parameter (in [0, 1]).
 * @return Point New point.
 */
Point lerp(const Point &p1, const Point &p2, const float t);

//----------------------------------------
// Grid functions
//----------------------------------------

/**
 * @brief Expand grid by translating and copying the values of the current
 * bounding box to the 8 first neighboring bounding boxes.
 *
 * @param x `x` coordinates.
 * @param y `y` coordinates.
 * @param value values.
 * @param bbox Bounding box.
 */
void expand_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 std::vector<float> &value,
                 Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Expand the grid by adding points on the boundaries of the bounding
 * box.
 *
 * @param x `x` coordinates.
 * @param y `y` coordinates.
 * @param value values.
 * @param bbox Bounding box.
 * @param corner_value Value at the boundary points.
 */
void expand_grid_boundaries(std::vector<float> &x,
                            std::vector<float> &y,
                            std::vector<float> &value,
                            Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f},
                            float               boundary_value = 0.f);

/**
 * @brief Expand the grid by adding four points at the corner of the bounding
 * box.
 *
 * @param x `x` coordinates.
 * @param y `y` coordinates.
 * @param value values.
 * @param bbox Bounding box.
 * @param corner_value Value at the four corner points.
 */
void expand_grid_corners(std::vector<float> &x,
                         std::vector<float> &y,
                         std::vector<float> &value,
                         Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f},
                         float               corner_value = 0.f);

/**
 * @brief
 *
 * @param array Input array.
 * @param x `x` coordinates (output).
 * @param y `y` coordinates (output).
 * @param bbox Bounding box.
 * @param threshold Theshold 'background' value.
 *
 * **Example**
 * @include ex_grid_from_array.cpp
 *
 * **Result**
 * @image html ex_grid_from_array.png
 */
void grid_from_array(Array              &array,
                     std::vector<float> &x,
                     std::vector<float> &y,
                     std::vector<float> &value,
                     Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f},
                     float               threshold = 0.f);

/**
 * @brief Generate a random grid.
 *
 * @param x[out] `x` coordinates (output).
 * @param y[out] `y` coordinates (output).
 * @param value[out] Random value, in [0, 1], at (x, y).
 * @param seed Random seed number.
 * @param bbox Bounding box.
 */
void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 std::vector<float> &value,
                 uint                seed,
                 Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f});

void random_grid(std::vector<float> &x,
                 std::vector<float> &y,
                 uint                seed,
                 Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f}); ///< @overload

/**
 * @brief
 *
 * @param x
 * @param y
 * @param density
 * @param seed
 * @param bbox
 */
void random_grid_density(std::vector<float> &x,
                         std::vector<float> &y,
                         Array              &density,
                         uint                seed,
                         Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Generate a jittered random grid.
 *
 * @param x[out] `x` coordinates (output).
 * @param y[out] `y` coordinates (output).
 * @param scale Jittering scale, in [0, 1].
 * @param seed Random seed number.
 * @param bbox Bounding box.
 */
void random_grid_jittered(std::vector<float> &x,
                          std::vector<float> &y,
                          float               scale,
                          uint                seed,
                          Vec4<float>         bbox = {0.f, 1.f, 0.f, 1.f});

/**
 * @brief Sort points in an ascending order (x then y).
 *
 * @param points Points to be sorted (in place).
 */
void sort_points(std::vector<Point> &points);

} // namespace hmap
