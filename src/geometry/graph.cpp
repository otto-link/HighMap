/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

std::vector<int> Graph::dijkstra(int source_point_index, int target_point_index)
{
  std::vector<float> dist(this->get_npoints());
  std::vector<int>   prev(this->get_npoints());

  // --- Dijkstra's algo
  std::list<int> queue = {};

  for (size_t i = 0; i < this->get_npoints(); i++)
  {
    dist[i] = std::numeric_limits<float>::max();
    prev[i] = -1;
    queue.push_back(i);
  }
  dist[source_point_index] = 0.f;

  while (queue.size() > 0)
  {
    // find closest point, within the queue
    int   i = 0;
    float dmax = std::numeric_limits<float>::max();
    for (auto &k : queue)
      if (dist[k] < dmax)
      {
        dmax = dist[k];
        i = k;
      }

    if (i == target_point_index)
      break;

    queue.remove(i);

    // loop over point i neighbors
    for (int &k : this->connectivity[i])
    {
      // check if the neighbor is in the queue
      bool found = (std::find(queue.begin(), queue.end(), k) != queue.end());

      if (found)
      {
        float alt = dist[i] + this->adjacency_matrix[{i, k}];
        if (alt < dist[k]) // alternative route is better
        {
          dist[k] = alt;
          prev[k] = i;
        }
      }
    }
  }

  // --- backward rebuild the complete path
  int              i = target_point_index;
  std::vector<int> path = {i};

  while (prev[i] > 0)
  {
    i = prev[i];
    path.push_back(i);
  }
  path.push_back(source_point_index);
  std::reverse(path.begin(), path.end());

  return path;
}

float Graph::get_edge_length(int k)
{
  return distance(this->points[this->edges[k][0]],
                  this->points[this->edges[k][1]]);
}

std::vector<float> Graph::get_lengths()
{
  std::vector<float> lengths = {};
  for (size_t k = 0; k < this->get_nedges(); k++)
  {
    lengths.push_back(distance(this->points[this->edges[k][0]],
                               this->points[this->edges[k][1]]));
  }
  return lengths;
}

std::vector<float> Graph::get_edge_x_pairs()
{
  std::vector<float> x;
  x.reserve(2 * this->get_nedges());
  for (auto &e : this->edges)
  {
    x.push_back(this->points[e[0]].x);
    x.push_back(this->points[e[1]].x);
  }
  return x;
}

std::vector<float> Graph::get_edge_y_pairs()
{
  std::vector<float> y;
  y.reserve(2 * this->get_nedges());
  for (auto &e : this->edges)
  {
    y.push_back(this->points[e[0]].y);
    y.push_back(this->points[e[1]].y);
  }
  return y;
}

Graph Graph::minimum_spanning_tree_prim()
{
  std::vector<int>   parent(this->get_npoints());
  std::vector<float> key(this->get_npoints());
  std::vector<bool>  is_point_in_mst(this->get_npoints());

  for (size_t i = 0; i < this->get_npoints(); i++)
  {
    key[i] = std::numeric_limits<float>::max();
    is_point_in_mst[i] = false;
  }

  // starting point
  key[0] = 0.f;
  parent[0] = -1;

  for (size_t i = 0; i < this->get_npoints() - 1; i++)
  {
    // find point with smallest 'key' while not being in the MS tree
    int   k = 0;
    float key_max = std::numeric_limits<float>::max();
    for (size_t p = 0; p < key.size(); p++)
      if ((key[p] < key_max) and (is_point_in_mst[p] == false))
      {
        key_max = key[p];
        k = p;
      }

    is_point_in_mst[k] = true;

    for (size_t p = 0; p < this->get_npoints(); p++)
    {
      if ((this->adjacency_matrix[{k, p}] > 0.f) and
          (is_point_in_mst[p] == false) and
          (this->adjacency_matrix[{k, p}] < key[p]))
      {
        parent[p] = k;
        key[p] = this->adjacency_matrix[{k, p}];
      }
    }
  }

  // build output graph
  Graph graph = Graph(this->points);
  for (size_t i = 1; i < this->get_npoints(); i++)
    graph.add_edge({(int)i, parent[i]});

  return graph;
}

void Graph::print()
{
  std::cout << "Points:" << std::endl;
  for (size_t k = 0; k < this->get_npoints(); k++)
  {
    std::cout << std::setw(6) << k;
    std::cout << std::setw(12) << this->points[k].x;
    std::cout << std::setw(12) << this->points[k].y;
    std::cout << std::setw(12) << this->points[k].v;
    std::cout << std::endl;
  }

  std::cout << "Edges: (index, {pt1, pt2}, weight)" << std::endl;
  for (size_t k = 0; k < this->get_nedges(); k++)
  {
    std::cout << std::setw(6) << k;
    std::cout << " {" << this->edges[k][0] << ", ";
    std::cout << this->edges[k][1] << "} ";
    std::cout << std::setw(12) << this->weights[k];
    std::cout << std::endl;
  }
}

Graph Graph::remove_orphan_points()
{
  Graph            graph_out = Graph();
  std::vector<int> new_point_idx(this->get_npoints());

  // fill vector with '-1' to keep track of which points have already
  // been added
  std::fill(new_point_idx.begin(), new_point_idx.end(), -1);

  this->update_connectivity();

  for (size_t k = 0; k < this->get_npoints(); k++)
  {
    if (this->connectivity[k].size() > 0)
    {
      // current point is connected to at least one other node =>
      // add it
      if (new_point_idx[k] == -1)
      {
        graph_out.add_point(this->points[k]);
        new_point_idx[k] = graph_out.get_npoints() - 1;
      }

      for (size_t r = 0; r < this->connectivity[k].size(); r++)
      {
        int j = this->connectivity[k][r];
        if ((j > (int)k) and (new_point_idx[j] == -1))
        {
          graph_out.add_point(this->points[j]);
          new_point_idx[j] = graph_out.get_npoints() - 1;
        }
      }
    }
  }

  // rebuild connectivity
  for (size_t k = 0; k < this->get_nedges(); k++)
  {
    int k1 = new_point_idx[this->edges[k][0]];
    int k2 = new_point_idx[this->edges[k][1]];
    graph_out.add_edge({k1, k2}, this->weights[k]);
  }

  return graph_out;
}

void Graph::to_array(Array &array, Vec4<float> bbox, bool color_by_edge_weight)
{
  if (color_by_edge_weight)
    for (std::size_t k = 0; k < this->get_nedges(); k++)
    {
      Point p1 = this->points[this->edges[k][0]];
      Point p2 = this->points[this->edges[k][1]];
      p1.v = this->weights[this->edges[k][0]];
      p2.v = this->weights[this->edges[k][1]];
      Path path = Path({p1, p2});
      path.to_array(array, bbox);
    }
  else
    for (std::size_t k = 0; k < this->get_nedges(); k++)
    {
      Point p1 = this->points[this->edges[k][0]];
      Point p2 = this->points[this->edges[k][1]];
      Path  path = Path({p1, p2});
      path.to_array(array, bbox);
    }
}

void Graph::to_array_fractalize(Array      &array,
                                Vec4<float> bbox,
                                int         iterations,
                                uint        seed,
                                float       sigma,
                                int         orientation,
                                float       persistence)
{
  // find smallest edge length
  float dmin = std::numeric_limits<float>::max();

  for (size_t k = 0; k < this->get_nedges(); k++)
  {
    float dist = this->get_edge_length(k);
    if (dist < dmin)
      dmin = dist;
  }

  // fractalize and project to array
  for (std::size_t k = 0; k < this->get_nedges(); k++)
  {
    Point p1 = this->points[this->edges[k][0]];
    Point p2 = this->points[this->edges[k][1]];
    Path  path = Path({p1, p2});

    path.resample(dmin);
    path.fractalize(iterations, seed, sigma, orientation, persistence);
    path.to_array(array, bbox);
  }
}

Array Graph::to_array_sdf(Vec2<int>   shape,
                          Vec4<float> bbox,
                          Array      *p_noise_x,
                          Array      *p_noise_y,
                          Vec2<float> shift,
                          Vec2<float> scale)
{
  // nodes
  std::vector<float> xp = this->get_edge_x_pairs();
  std::vector<float> yp = this->get_edge_y_pairs();

  for (size_t k = 0; k < xp.size(); k++)
  {
    xp[k] = (xp[k] - bbox.a) / (bbox.b - bbox.a);
    yp[k] = (yp[k] - bbox.c) / (bbox.d - bbox.c);
  }

  // fill heightmap
  auto distance_fct = [&xp, &yp](float x, float y)
  {
    float d = std::numeric_limits<float>::max();

    for (size_t i = 0; i < xp.size() - 1; i += 2)
    {
      size_t      j = i + 1;
      Vec2<float> e = {xp[j] - xp[i], yp[j] - yp[i]};
      Vec2<float> w = {x - xp[i], y - yp[i]};
      float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
      Vec2<float> b = {w.x - e.x * coeff, w.y - e.y * coeff};
      d = std::min(d, dot(b, b));
    }
    return std::sqrt(d);
  };

  Array z = sdf_generic(shape,
                        distance_fct,
                        p_noise_x,
                        p_noise_y,
                        Vec2<float>(0.f, 0.f), // center at bottom-left
                        shift,
                        scale);

  return z;
}

void Graph::to_csv(std::string fname_xy, std::string fname_adjacency)
{
  std::fstream f;

  f.open(fname_xy, std::ios::out);
  for (auto &p : this->points)
    f << p.x << "," << p.y << "," << p.v << std::endl;
  f.close();

  f.open(fname_adjacency, std::ios::out);
  for (int i = 0; i < (int)this->get_npoints(); i++)
  {
    for (int j = 0; j < (int)this->get_npoints(); j++)
    {
      float v = 0.f;
      if (this->adjacency_matrix.count({i, j}))
        v = this->adjacency_matrix[{i, j}];
      f << v;

      if (j < (int)this->get_npoints() - 1)
        f << ",";
    }
    f << std::endl;
  }

  f.close();
}

void Graph::to_png(std::string fname, Vec2<int> shape)
{
  Array array = Array(shape);
  this->to_array(array, this->get_bbox());
  array.to_png(fname, cmap::inferno, false);
}

void Graph::update_adjacency_matrix()
{
  this->adjacency_matrix.clear();

  // fill matrix
  for (std::size_t k = 0; k < this->get_nedges(); k++)
  {
    this->adjacency_matrix[{this->edges[k][0], this->edges[k][1]}] =
        this->weights[k];
    this->adjacency_matrix[{this->edges[k][1], this->edges[k][0]}] =
        this->weights[k];
  }
}

void Graph::update_connectivity()
{
  std::vector<std::vector<int>> nbrs(this->get_npoints());

  for (std::size_t k = 0; k < this->get_nedges(); k++)
  {
    nbrs[this->edges[k][0]].push_back(this->edges[k][1]);
    nbrs[this->edges[k][1]].push_back(this->edges[k][0]);
  }

  this->connectivity = nbrs;
}

} // namespace hmap
