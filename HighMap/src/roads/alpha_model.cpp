/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "op/vector_utils.hpp"

namespace hmap
{

Graph generate_network_alpha_model(std::vector<float> xc,
                                   std::vector<float> yc,
                                   std::vector<float> size,
                                   Vec4<float>        bbox,
                                   Array             &z,
                                   uint               seed,
                                   float              alpha,
                                   int                n_dummy_nodes,
                                   float              dz_weight,
                                   Array             *p_weight)
{
  size_t nc = xc.size();

  //--- tesselation: randomly add "dummy" nodes and use Delaunay
  //--- triangulation to create a mesh

  Graph graph = Graph();
  {
    std::vector<float> x(n_dummy_nodes);
    std::vector<float> y(n_dummy_nodes);
    random_grid_jittered(x, y, 0.4f, seed, bbox);
    Cloud cloud = Cloud(x, y);

    for (size_t k = 0; k < nc; k++)
    {
      Point p = Point(xc[k], yc[k], size[k]);
      cloud.add_point(p);
    }

    // Delanay triangulation
    graph = cloud.to_graph_delaunay();
    graph.set_values_from_array(z, bbox);
    graph.update_connectivity();
  }

  //--- road weights

  Array is_road = Array(
      Vec2<int>((int)graph.get_npoints(), (int)graph.get_npoints()));

  // define number of trips between each cities
  std::vector<float> ntrips = {};
  std::vector<int>   trips_istart = {};
  std::vector<int>   trips_iend = {};

  for (size_t i = 0; i < nc; i++)
    for (size_t j = i + 1; j < nc; j++)
    {
      float dist = (xc[i] - xc[j]) * (xc[i] - xc[j]) +
                   (yc[i] - yc[j]) * (yc[i] - yc[j]);

      ntrips.push_back(size[i] * size[j] / (1.f + dist));
      trips_istart.push_back(i);
      trips_iend.push_back(j);
    }

  // compute adjacency matrix based on the Euclidian distance
  // between points and add elevation difference
  graph.update_adjacency_matrix();

  //
  std::vector<float> local_weight(graph.get_npoints());
  if (p_weight != nullptr)
    local_weight = graph.interpolate_values_from_array(*p_weight, bbox);

  for (size_t i = 0; i < graph.get_npoints(); i++)
    for (size_t r = 0; r < graph.connectivity[i].size(); r++)
    {
      int j = graph.connectivity[i][r];
      if (j > (int)i)
      {
        float dz = graph.points[i].v - graph.points[j].v;
        graph.adjacency_matrix[{i, j}] += std::abs(dz) * dz_weight;
        graph.adjacency_matrix[{i, j}] += local_weight[i] + local_weight[j];
        graph.adjacency_matrix[{j, i}] = graph.adjacency_matrix[{i, j}];
      }
    }

  // start with the most important connections
  std::vector<size_t> ksort = argsort(ntrips);

  for (size_t k = ntrips.size() - 1; k-- > 0;)
  {
    int i0 = (int)graph.get_npoints() - nc + trips_istart[ksort[k]];
    int j0 = (int)graph.get_npoints() - nc + trips_iend[ksort[k]];

    // shortest path between the two cities (i0 and j0)
    std::vector<int> path = graph.dijkstra(i0, j0);

    // update road/non-road status
    for (size_t i = 0; i < path.size() - 1; i++)
    {
      int i1 = std::min(path[i], path[i + 1]);
      int i2 = std::max(path[i], path[i + 1]);
      is_road(i1, i2) += 1.f;
    }

    // weight adjacency matrix using elevation difference and
    // road/non-road type of the edge
    for (size_t i = 0; i < graph.get_npoints(); i++)
      // loop over neighbors
      for (size_t r = 0; r < graph.connectivity[i].size(); r++)
      {
        int j = graph.connectivity[i][r];
        if ((j > (int)i) and (is_road(i, j) == 1))
        {
          graph.adjacency_matrix[{i, j}] *= alpha;
          graph.adjacency_matrix[{j, i}] = graph.adjacency_matrix[{i, j}];
        }
      }
  }

  //--- remove orphan edges and rebuild road network graph
  Graph network = Graph(graph.get_x(), graph.get_y());

  for (size_t i = 0; i < graph.get_npoints(); i++)
    for (size_t r = 0; r < graph.connectivity[i].size(); r++)
    {
      int j = graph.connectivity[i][r];
      if ((j > (int)i) and (is_road(i, j) > 0))
        network.add_edge({(int)i, j}, is_road((int)i, j));
    }

  // store city size in node value (equals to 0 if the node is not a
  // city)
  for (size_t i = 0; i < network.get_npoints(); i++)
    if (i < network.get_npoints() - nc)
      network.points[i].v = 0.f;
    else
      network.points[i].v = size[i - network.get_npoints() + nc];

  // final clean-up
  network = network.remove_orphan_points();
  network.update_adjacency_matrix();

  return network;
}

} // namespace hmap
