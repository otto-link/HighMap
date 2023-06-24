#include <cmath>
#include <fstream>
#include <iomanip>
#include <string>

#include "delaunator.hpp"
#include "macrologger.h"

#include "highmap/geometry.hpp"

namespace hmap
{

Cloud::Cloud(int npoints, uint seed, std::vector<float> bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis;

  for (int k = 0; k < npoints; k++)
  {
    Point p(dis(gen), dis(gen), dis(gen));
    this->add_point(p);
  }
  this->remap_xy(bbox);
};

void Cloud::print()
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

  std::vector<float> bbox = this->get_bbox();
  std::cout << "   bounding box: {" << bbox[0] << ", " << bbox[1] << ", "
            << bbox[2] << ", " << bbox[3] << "}" << std::endl;
}

void Cloud::remap_xy(std::vector<float> bbox_new)
{
  std::vector<float> bbox = this->get_bbox();
  for (auto &p : this->points)
  {
    p.x = (p.x - bbox[0]) / (bbox[1] - bbox[0]) * (bbox_new[1] - bbox_new[0]) +
          bbox_new[0];
    p.y = (p.y - bbox[2]) / (bbox[3] - bbox[2]) * (bbox_new[3] - bbox_new[2]) +
          bbox_new[2];
  }
}

void Cloud::set_values_from_chull_distance()
{
  for (size_t i = 0; i < this->get_npoints(); i++)
  {
    float dmax = std::numeric_limits<float>::max();
    for (size_t k = 0; k < this->convex_hull.size(); k++)
    {
      float dist =
          distance(this->points[i], this->points[this->convex_hull[k]]);
      if (dist < dmax)
      {
        dmax = dist;
        this->points[i].v = dist;
      }
    }
  }
}

void Cloud::to_array(Array &array, std::vector<float> bbox)
{
  int   ni = array.shape[0];
  int   nj = array.shape[1];
  float ai = (ni - 1) / (bbox[1] - bbox[0]);
  float bi = -bbox[0] * (ni - 1) / (bbox[1] - bbox[0]);
  float aj = (nj - 1) / (bbox[3] - bbox[2]);
  float bj = -bbox[2] * (nj - 1) / (bbox[3] - bbox[2]);

  for (auto &p : this->points)
  {
    int i = (int)(ai * p.x + bi);
    int j = (int)(aj * p.y + bj);

    if ((i > -1) and (i < ni) and (j > -1) and (j < nj))
      array(i, j) = p.v;
  }
}

void Cloud::to_csv(std::string fname)
{
  std::fstream f;
  f.open(fname, std::ios::out);
  for (auto &p : this->points)
    f << p.x << "," << p.y << "," << p.v << std::endl;
  f.close();
}

Graph Cloud::to_graph_delaunay()
{
  std::vector<float>     coords = this->get_xy();
  delaunator::Delaunator d(coords);

  Graph graph = Graph(*this);

  for (std::size_t e = 0; e < d.triangles.size(); e++)
  {
    int i = d.halfedges[e];
    if (((int)e > i) or (i == -1))
    {
      int next_he = (e % 3 == 2) ? e - 2 : e + 1;
      graph.add_edge({(int)d.triangles[e], (int)d.triangles[next_he]});
    }
  }

  // store convex hull indices
  graph.convex_hull = {(int)d.hull_start};

  int inext = d.hull_next[graph.convex_hull.back()];
  while (inext != graph.convex_hull[0])
  {
    graph.convex_hull.push_back(inext);
    inext = d.hull_next[graph.convex_hull.back()];
  }

  return graph;
}

} // namespace hmap
