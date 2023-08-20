/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <fstream>
#include <iomanip>
#include <string>

#include "AnyInterpolator.hpp"
#include "Interpolate.hpp"
#include "macrologger.h"

#include "highmap/geometry.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Cloud::Cloud(int npoints, uint seed, Vec4<float> bbox)
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

std::vector<float> Cloud::interpolate_values_from_array(const Array &array,
                                                        Vec4<float>  bbox)
{
  std::vector<float> v_out(this->get_npoints());
  float              ax = (float)(array.shape.x - 1) / (bbox.b - bbox.a);
  float bx = (float)(array.shape.x - 1) * (-bbox.a) / (bbox.b - bbox.a);
  float ay = (float)(array.shape.y - 1) / (bbox.d - bbox.c);
  float by = (float)(array.shape.y - 1) * (-bbox.c) / (bbox.d - bbox.c);

  for (size_t k = 0; k < this->get_npoints(); k++)
  {
    float x = ax * this->points[k].x + bx;
    float y = ay * this->points[k].y + by;
    int   i = (int)x;
    int   j = (int)y;

    if ((i > -1) and (i < array.shape.x) and (j > -1) and (j < array.shape.y))
    {
      float u = x - (float)i;
      float v = y - (float)j;
      v_out[k] = array.get_value_bilinear_at(i, j, u, v);
    }
  }
  return v_out;
}

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

  Vec4<float> bbox = this->get_bbox();
  std::cout << "   bounding box: {" << bbox.a << ", " << bbox.b << ", "
            << bbox.c << ", " << bbox.d << "}" << std::endl;
}

void Cloud::remap_xy(Vec4<float> bbox_new)
{
  Vec4<float> bbox = this->get_bbox();
  for (auto &p : this->points)
  {
    p.x = (p.x - bbox.a) / (bbox.b - bbox.a) * (bbox_new.b - bbox_new.a) +
          bbox_new.a;
    p.y = (p.y - bbox.c) / (bbox.d - bbox.c) * (bbox_new.d - bbox_new.c) +
          bbox_new.c;
  }
}

void Cloud::set_values_from_array(const Array &array, Vec4<float> bbox)
{
  float ax = (float)(array.shape.x - 1) / (bbox.b - bbox.a);
  float bx = (float)(array.shape.x - 1) * (-bbox.a) / (bbox.b - bbox.a);
  float ay = (float)(array.shape.y - 1) / (bbox.d - bbox.c);
  float by = (float)(array.shape.y - 1) * (-bbox.c) / (bbox.d - bbox.c);

  for (size_t k = 0; k < this->get_npoints(); k++)
  {
    float x = ax * this->points[k].x + bx;
    float y = ay * this->points[k].y + by;
    int   i = (int)x;
    int   j = (int)y;

    if ((i > -1) and (i < array.shape.x) and (j > -1) and (j < array.shape.y))
    {
      float u = x - (float)i;
      float v = y - (float)j;
      this->points[k].v = array.get_value_bilinear_at(i, j, u, v);
    }
  }
}

void Cloud::set_values_from_chull_distance()
{
  for (size_t i = 0; i < this->get_npoints(); i++)
  {
    float dmax = std::numeric_limits<float>::max();
    for (size_t k = 0; k < this->convex_hull.size(); k++)
    {
      float dist = distance(this->points[i],
                            this->points[this->convex_hull[k]]);
      if (dist < dmax)
      {
        dmax = dist;
        this->points[i].v = dist;
      }
    }
  }
}

void Cloud::to_array(Array &array, Vec4<float> bbox)
{
  int   ni = array.shape.x;
  int   nj = array.shape.y;
  float ai = (ni - 1) / (bbox.b - bbox.a);
  float bi = -bbox.a * (ni - 1) / (bbox.b - bbox.a);
  float aj = (nj - 1) / (bbox.d - bbox.c);
  float bj = -bbox.c * (nj - 1) / (bbox.d - bbox.c);

  for (auto &p : this->points)
  {
    int i = (int)(ai * p.x + bi);
    int j = (int)(aj * p.y + bj);

    if ((i > -1) and (i < ni) and (j > -1) and (j < nj))
      array(i, j) = p.v;
  }
}

void Cloud::to_array_interp(Array      &array,
                            Vec4<float> bbox,
                            int         interpolation_method,
                            Array      *p_noise_x,
                            Array      *p_noise_y,
                            Vec2<float> shift,
                            Vec2<float> scale)
{
  std::vector<float> x = this->get_x();
  std::vector<float> y = this->get_y();
  std::vector<float> v = this->get_values();

  expand_grid(x, y, v, bbox);

  _2D::AnyInterpolator<
      float,
      void(std::vector<float>, std::vector<float>, std::vector<float>)>
      interp; // = _2D::LinearDelaunayTriangleInterpolator<float>();

  if (interpolation_method == 0)
    interp = _2D::LinearDelaunayTriangleInterpolator<float>();
  else if (interpolation_method == 1)
    interp = _2D::ThinPlateSplineInterpolator<float>();
  else
    LOG_ERROR("unknown interpolation method");

  interp.setData(x, y, v);

  // array grid
  Vec2<int> shape = array.shape;
  float     lx = bbox.b - bbox.a;
  float     ly = bbox.d - bbox.c;

  std::vector<float> xg = linspace(bbox.a + shift.x * lx,
                                   bbox.a + (shift.x + scale.x) * lx,
                                   shape.x);
  std::vector<float> yg = linspace(bbox.c + shift.y * ly,
                                   bbox.c + (shift.y + scale.y) * ly,
                                   shape.y);

  helper_get_noise(array,
                   xg,
                   yg,
                   p_noise_x,
                   p_noise_y,
                   [&interp](float x_, float y_) { return interp(x_, y_); });
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
  std::vector<float>            coords = this->get_xy();
  delaunator::Delaunator<float> d(coords);
  Graph                         graph = Graph(*this);

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
