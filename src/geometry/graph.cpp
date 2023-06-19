#include <iostream>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/io.hpp"

namespace hmap
{

std::vector<float> Graph::get_edge_lengths()
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

  std::cout << "Edges:" << std::endl;
  for (size_t k = 0; k < this->get_nedges(); k++)
  {
    std::cout << std::setw(6) << k;
    std::cout << " {" << this->edges[k][0] << ", ";
    std::cout << this->edges[k][1] << "}";
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

} // namespace hmap
