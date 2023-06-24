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
#include <random>
#include <vector>

#include "highmap/array.hpp"

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

  bool operator==(Point const &p) const
  {
    return (x == p.x) && (y == p.y);
  }
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
  Cloud(int npoints, uint seed, std::vector<float> bbox = {0.f, 1.f, 0.f, 1.f});

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
   */
  Cloud(std::vector<float> x, std::vector<float> y)
  {
    for (size_t k = 0; k < x.size(); k++)
    {
      Point p(x[k], y[k]);
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
  std::vector<float> get_bbox()
  {
    std::vector<float> x = this->get_x();
    std::vector<float> y = this->get_y();
    std::vector<float> bbox(4);
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
   * @brief Get the number of points.
   *
   * @return size_t Number of points.
   */
  size_t get_npoints()
  {
    return this->points.size();
  }

  /**
   * @brief Get the `x` of the points.
   *
   * @return std::vector<float> `x` values.
   */
  std::vector<float> get_x()
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
  std::vector<float> get_xy()
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
  std::vector<float> get_y()
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

  /**
   * @brief Set the values distance from chull object
   *
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
   * @brief Print some data.
   *
   */
  void print();

  /**
   * @brief Remap the values of the point coordinates.
   *
   * @param xmin New `xmin`.
   * @param xmax New `xmax`.
   * @param ymin New `ymin`.
   * @param ymax New `ymax`.
   */
  void remap_xy(std::vector<float> bbox_new);

  /**
   * @brief Project cloud points to an array.
   *
   * @param array Input array.
   * @param bbox Bounding box of the array.
   */
  void to_array(Array &array, std::vector<float> bbox = {0.f, 1.f, 0.f, 1.f});

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
  Array adjacency_matrix = Array({0, 0});

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

  /**
   * @brief Get the length of edge `k`.
   *
   * @param k Edge index.
   * @return float Euclidian length.
   */
  float get_edge_length(int k);

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
   * @brief Export graph as png image file.
   *
   * @param fname File name.
   * @param shape Image resolution.
   */
  void to_png(std::string fname, std::vector<int> shape = {512, 512});

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
  Path(int                npoints,
       uint               seed,
       std::vector<float> bbox = {0.f, 1.f, 0.f, 1.f},
       bool               closed = false)
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
  // methods
  //----------------------------------------

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
   * @brief Reorder points using a nearest neighbor search.
   *
   * @param start_index Starting point of the search.
   */
  void reorder_nns(int start_index = 0);

  /**
   * @brief Project cloud points to an array.
   *
   * @param array Input array.
   * @param bbox Bounding box of the array.
   */
  void to_array(Array &array, std::vector<float> bbox);

  /**
   * @brief Export path as png image file.
   *
   * @param fname File name.
   * @param shape Image resolution.
   */
  void to_png(std::string fname, std::vector<int> shape = {512, 512});

  /**
   * @brief Resample the path in order to get fairly uniform distance between
   * each consecutive points.
   *
   */
  void uniform_resampling();
};

/**
 * @brief Return the polar angle between two points.
 *
 * @param p1 1st point.
 * @param p2 2nd point.
 * @return float Angle in radians.
 */
float angle(const Point &p1, const Point &p2);

/**
 * @brief Return the linear interpolation between two points by a parameter `t`.
 *
 * @param p1 1st point.
 * @param p2 2nd point.
 * @param t Interpolation parameter (in [0, 1]).
 * @return Point New point.
 */
Point lerp(const Point &p1, const Point &p2, const float t);

/**
 * @brief Return the distance between two points in 2D, using only `x` and `y`
 * coordinates.
 *
 * @param p1 1st point.
 * @param p2 2nd point.
 * @return float Euclidian distance.
 */
float distance(const Point &p1, const Point &p2);

} // namespace hmap
