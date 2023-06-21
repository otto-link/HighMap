#include <algorithm>
#include <iostream>
#include <limits>
#include <list>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/io.hpp"

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
        float alt = dist[i] + this->adjacency_matrix(i, k);
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

void Graph::to_png(std::string fname, std::vector<int> shape)
{
  Array array = Array(shape);

  for (std::size_t k = 0; k < this->get_nedges(); k++)
  {
    Point p1 = this->points[this->edges[k][0]];
    Point p2 = this->points[this->edges[k][1]];
    Path  path = Path({p1, p2});
    path.to_array(array, this->get_bbox());
  }
  array.to_png(fname, cmap::gray, false);
}

void Graph::update_adjacency_matrix()
{
  Array mat = Array({(int)this->get_nedges(), (int)this->get_nedges()});

  for (std::size_t k = 0; k < this->get_nedges(); k++)
  {
    mat(this->edges[k][0], this->edges[k][1]) = this->weights[k];
    mat(this->edges[k][1], this->edges[k][0]) = this->weights[k];
  }

  this->adjacency_matrix = mat;
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
