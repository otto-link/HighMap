/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file graph.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Definition of the `Graph` class for manipulating 2D graphs.
 *
 * This file contains the definition of the `Graph` class, which extends from
 * the `Cloud` class to handle graph data structures in a 2D space. The `Graph`
 * class supports various functionalities such as graph visualization, edge
 * operations, graph algorithms (e.g., Dijkstra's algorithm, Minimum Spanning
 * Tree), and exporting data to different formats (CSV, PNG).
 *
 * The class includes methods for computing distances, generating visual
 * representations, and manipulating graph edges and nodes. It also provides
 * functionalities for exporting the graph data to CSV and PNG formats, as well
 * as for calculating Signed Distance Functions (SDF) of the graph.
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
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/point.hpp"

namespace hmap
{

class Cloud;

/**
 * @brief Graph class, to manipulate graphs in 2D.
 *
 * This class represents a 2D graph, allowing the creation, manipulation, and
 * analysis of graphs derived from point clouds. It supports operations such as
 * graph construction, traversal, and various geometric analyses. This class
 * inherits from the `Cloud` class, leveraging the functionalities of point
 * clouds while adding graph-specific methods.
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
   * This member variable stores the edges of the graph. Each edge is
   * represented as a pair of indices referring to the vertices in the graph.
   * The edges are stored as a vector of vectors, where each inner vector
   * contains the indices of vertices connected by that edge.
   */
  std::vector<std::vector<int>> edges = {};

  /**
   * @brief Edge weights.
   *
   * This member variable stores the weights associated with the edges of the
   * graph. Each weight corresponds to an edge and is stored in a vector. The
   * weights are used to represent the cost or distance associated with
   * traveling along an edge in the graph.
   */
  std::vector<float> weights = {};

  /**
   * @brief Store point connectivity.
   *
   * This member variable stores the connectivity information of the points in
   * the graph. It is represented as a vector of vectors, where each inner
   * vector contains the indices of neighboring vertices connected to the
   * corresponding point.
   */
  std::vector<std::vector<int>> connectivity = {};

  /**
   * @brief Adjacency matrix.
   *
   * This member variable represents the adjacency matrix of the graph. It is a
   * map where each key is a pair of vertex indices and the value is the weight
   * of the edge connecting those vertices. The adjacency matrix provides a way
   * to quickly access the weight of an edge between any two vertices.
   */
  std::map<std::pair<int, int>, float> adjacency_matrix;

  /**
   * @brief Construct a new Graph object.
   *
   * The default constructor initializes a new `Graph` object. It calls the base
   * class constructor `Cloud()` to set up any inherited functionality from the
   * `Cloud` class. This constructor sets up an empty graph with no edges,
   * weights, connectivity, or adjacency matrix.
   */
  Graph() : Cloud(){};

  /**
   * @brief Construct a new Graph object based on a cloud of points.
   *
   * This constructor initializes a `Graph` object using a `Cloud` object. The
   * `Cloud` object provides the points which will be used to construct the
   * graph. This constructor is useful when you have a `Cloud` object and want
   * to create a `Graph` representation from it.
   *
   * @param cloud The cloud of points used to initialize the graph.
   */
  Graph(Cloud cloud) : Cloud(cloud){};

  /**
   * @brief Construct a new Graph object based on a list of points.
   *
   * This constructor initializes a `Graph` object using a vector of `Point`
   * objects. The `Point` objects are used to populate the vertices of the
   * graph. This constructor is useful when you have a list of `Point` objects
   * and want to create a `Graph` representation from them.
   *
   * @param points The list of points used to initialize the graph.
   */
  Graph(std::vector<Point> points) : Cloud(points){};

  /**
   * @brief Construct a new Graph object based on x and y coordinates.
   *
   * This constructor initializes a `Graph` object using separate vectors for x
   * and y coordinates. The points are created from these coordinates and used
   * to populate the vertices of the graph. This constructor is useful when you
   * have x and y coordinates and want to create a `Graph` representation from
   * them.
   *
   * @param x Vector of x coordinates for the points.
   * @param y Vector of y coordinates for the points.
   */
  Graph(std::vector<float> x, std::vector<float> y) : Cloud(x, y){};

  /**
   * @brief Add an edge to the graph.
   *
   * This method adds a new edge to the graph. The edge is specified by a vector
   * of two indices representing the points connected by the edge. The weight of
   * the edge can be provided explicitly; if not provided, the Euclidean
   * distance between the connected points is used as the default weight.
   *
   * @param edge A vector of two integers representing the indices of the points
   * connected by the edge.
   * @param weight The weight of the edge. If not provided, the default weight
   * is calculated as the Euclidean distance between the points.
   */
  void add_edge(std::vector<int> edge, float weight);

  /**
   * @brief Add an edge to the graph with default weight.
   *
   * This method adds a new edge to the graph. The edge is specified by a vector
   * of two indices representing the points connected by the edge. The weight of
   * the edge is calculated as the Euclidean distance between the connected
   * points.
   *
   * @param edge A vector of two integers representing the indices of the points
   * connected by the edge.
   */
  void add_edge(std::vector<int> edge);

  /**
   * @brief Return the shortest route between two points using Dijkstra's
   * algorithm.
   *
   * This method computes the shortest path from a specified source point to a
   * target point in the graph using Dijkstra's algorithm. It returns a vector
   * of point indices representing the route from the source to the target
   * point.
   *
   * @param source_point_index The index of the starting point in the graph.
   * @param target_point_index The index of the ending point in the graph.
   * @return std::vector<int> A vector of point indices representing the
   * shortest path from the source to the target.
   *
   * **Example**
   * @include ex_graph_dijkstra.cpp
   */
  std::vector<int> dijkstra(int source_point_index, int target_point_index);

  /**
   * @brief Get the length of edge `k`.
   *
   * This method calculates the Euclidean length of a specific edge in the
   * graph. The edge is identified by the index `k`, and its length is
   * determined by the distance between the two vertices connected by the edge.
   *
   * @param k Index of the edge for which the length is to be computed.
   * @return float The Euclidean length of the edge.
   */
  float get_edge_length(int k);

  /**
   * @brief Return x coordinates of the edges (as pairs).
   *
   * This method returns the x coordinates of the endpoints of each edge in the
   * graph. The coordinates are returned as a vector of floats, where each pair
   * of floats represents the x coordinates of an edge's endpoints.
   *
   * @return std::vector<float> The x coordinates of the edges.
   */
  std::vector<float> get_edge_x_pairs();

  /**
   * @brief Return y coordinates of the edges (as pairs).
   *
   * This method returns the y coordinates of the endpoints of each edge in the
   * graph. The coordinates are returned as a vector of floats, where each pair
   * of floats represents the y coordinates of an edge's endpoints.
   *
   * @return std::vector<float> The y coordinates of the edges.
   */
  std::vector<float> get_edge_y_pairs();

  /**
   * @brief Get the length of all the edges.
   *
   * This method returns the lengths of all the edges in the graph. The lengths
   * are computed using the Euclidean distance formula, and the result is
   * returned as a vector of floats.
   *
   * @return std::vector<float> The lengths of all the edges in the graph.
   */
  std::vector<float> get_lengths();

  /**
   * @brief Get the number of edges in the graph.
   *
   * This method returns the total number of edges present in the graph. The
   * edges are stored in a vector, and this method returns the size of that
   * vector, which represents the number of edges.
   *
   * @return size_t The number of edges in the graph.
   */
  size_t get_nedges();

  /**
   * @brief Generate a Minimum Spanning Tree (MST) of the graph using Prim's
   * algorithm.
   *
   * This method creates a Minimum Spanning Tree from the graph using Prim's
   * algorithm. It returns a new `Graph` object that represents the MST, which
   * connects all the points in the graph with the minimum total edge weight.
   *
   * @return Graph The Minimum Spanning Tree (MST) of the original graph.
   *
   * **Example**
   * @include ex_graph_minimum_spanning_tree_prim.cpp
   * **Result**
   * @image html ex_graph_minimum_spanning_tree_prim0.png
   * @image html ex_graph_minimum_spanning_tree_prim1.png
   */
  Graph minimum_spanning_tree_prim();

  /**
   * @brief Print the graph data to the standard output.
   *
   * This method prints the current state of the graph, including point
   * coordinates, edges, and edge weights.
   */
  void print();

  /**
   * @brief Remove orphan points from the graph.
   *
   * Orphan points are points that are not connected to any edges. This method
   * removes such points from the graph and returns a new `Graph` object that
   * excludes these orphan points.
   *
   * @return Graph A new graph object with orphan points removed.
   */
  Graph remove_orphan_points();

  /**
   * @brief Project the graph to an array and optionally color by edge weight.
   *
   * This method projects the graph onto a 2D array. The array's elements are
   * filled based on the graph's structure, and optionally, the color can
   * represent edge weights. This allows visual representation of the graph in
   * array form.
   *
   * @param array The input array to project the graph onto.
   * @param bbox The bounding box for the array.
   * @param color_by_edge_weight If `true`, colors the array based on edge
   * weights; otherwise, colors by node values.
   */
  void to_array(Array      &array,
                Vec4<float> bbox,
                bool        color_by_edge_weight = true);

  /**
   * @brief Apply fractalization to graph edges and project to an array.
   *
   * This method applies a fractalization process to the graph edges, creating a
   * more complex structure, and then projects the result onto an array. The
   * fractalization includes multiple iterations and random Gaussian
   * displacement to generate a fractal effect. The parameters control the
   * number of iterations, randomness, and how the graph path is altered.
   *
   * @param array The input array to project the fractalized graph onto.
   * @param bbox The bounding box for the array.
   * @param iterations Number of fractalization iterations to perform.
   * @param seed Random seed number for stochastic processes.
   * @param sigma Half-width of the Gaussian displacement normalized by the
   * distance between points.
   * @param orientation Displacement orientation: 0 for random inward/outward, 1
   * to inflate, -1 to deflate.
   * @param persistence Noise persistence factor with respect to iteration
   * number.
   */
  void to_array_fractalize(Array      &array,
                           Vec4<float> bbox,
                           int         iterations,
                           uint        seed,
                           float       sigma = 0.3f,
                           int         orientation = 0.f,
                           float       persistence = 1.f);

  /**
   * @brief Generate an array filled with the Signed Distance Function (SDF) to
   * the graph.
   *
   * This method computes the signed distance function for the graph, which
   * measures the distance of each point in the array to the nearest edge of the
   * graph. The result is projected onto an output array. The optional noise
   * arrays can be used for domain warping.
   *
   * @param shape The shape of the output array.
   * @param bbox The bounding box defining the area over which the SDF is
   * computed.
   * @param p_noise_x Reference to the input noise array for domain warping in
   * the x-direction (optional).
   * @param p_noise_y Reference to the input noise array for domain warping in
   * the y-direction (optional).
   * @param bbox_array The bounding box of the destination array.
   * @return Array The resulting array with the signed distance function values.
   *
   * **Example**
   * @include ex_cloud_sdf.cpp
   * **Result**
   * @image html ex_cloud_sdf.png
   */
  Array to_array_sdf(Vec2<int>   shape,
                     Vec4<float> bbox,
                     Array      *p_noise_x = nullptr,
                     Array      *p_noise_y = nullptr,
                     Vec4<float> bbox_array = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Export graph data to CSV files.
   *
   * This method exports the graph data to two separate CSV files: one for node
   * coordinates and one for the adjacency matrix. The node coordinates file
   * contains the `(x, y)` coordinates of the graph's nodes, and the adjacency
   * matrix file contains the graph's connectivity information.
   *
   * @param fname_xy Filename for the CSV file containing node `(x, y)`
   * coordinates.
   * @param fname_adjacency Filename for the CSV file containing the adjacency
   * matrix.
   */
  void to_csv(std::string fname_xy, std::string fname_adjacency);

  /**
   * @brief Export the graph as a PNG image file.
   *
   * This method exports a visual representation of the graph as a PNG image
   * file. The resolution of the image can be specified using the `shape`
   * parameter.
   *
   * @param fname The file name for the PNG image.
   * @param shape The resolution of the image in pixels (width, height).
   */
  void to_png(std::string fname, Vec2<int> shape = {512, 512});

  /**
   * @brief Update the adjacency matrix of the graph.
   *
   * This method updates the adjacency matrix based on the current graph edges
   * and weights. The adjacency matrix represents the connectivity between nodes
   * in the graph.
   */
  void update_adjacency_matrix();

  /**
   * @brief Update the point connectivity information.
   *
   * This method updates the point connectivity data, which describes the
   * relationships between nodes in the graph based on the current edges.
   */
  void update_connectivity();
};
} // namespace hmap