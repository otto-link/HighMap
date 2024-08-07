/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>
#include <fstream>
#include <iomanip>
#include <string>

#include "libInterpolate/AnyInterpolator.hpp"
#include "libInterpolate/Interpolate.hpp"
#include "macrologger.h"

#include "highmap/geometry.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Cloud::Cloud(int npoints, uint seed, Vec4<float> bbox)
{
  this->points.resize(npoints);
  this->randomize(seed, bbox);
};

void Cloud::clear()
{
  this->points.clear();
  this->convex_hull.clear();
}

float Cloud::get_values_max()
{
  std::vector<float> values = this->get_values();
  return *std::max_element(values.begin(), values.end());
}

float Cloud::get_values_min()
{
  std::vector<float> values = this->get_values();
  return *std::min_element(values.begin(), values.end());
}

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

void Cloud::randomize(uint seed, Vec4<float> bbox)
{
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis;

  for (auto &p : this->points)
  {
    p.x = dis(gen) * (bbox.b - bbox.a) + bbox.a;
    p.y = dis(gen) * (bbox.d - bbox.c) + bbox.c;
    p.v = dis(gen);
  }
}

void Cloud::remap_values(float vmin, float vmax)
{
  float from_min = this->get_values_min();
  float from_max = this->get_values_max();

  if (from_min != from_max)
    for (auto &p : this->points)
      p.v = (p.v - from_min) / (from_max - from_min) * (vmax - vmin) + vmin;
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
                            Vec4<float> bbox_array)
{
  std::vector<float> x = this->get_x();
  std::vector<float> y = this->get_y();
  std::vector<float> v = this->get_values();

  float       lx = bbox.b - bbox.a;
  float       ly = bbox.d - bbox.c;
  Vec4<float> bbox_expanded = {bbox.a - lx,
                               bbox.b + lx,
                               bbox.c - ly,
                               bbox.d + ly};
  expand_grid_corners(x, y, v, bbox_expanded, 0.f);

  _2D::AnyInterpolator<
      float,
      void(std::vector<float>, std::vector<float>, std::vector<float>)>
      interp;

  if (interpolation_method == 0)
    interp = _2D::LinearDelaunayTriangleInterpolator<float>();
  else if (interpolation_method == 1)
    interp = _2D::ThinPlateSplineInterpolator<float>();
  else
    LOG_ERROR("unknown interpolation method");

  interp.setData(x, y, v);

  fill_array_using_xy_function(array,
                               bbox_array,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               nullptr,
                               [&interp](float x, float y, float)
                               { return interp(x, y); });
}

Array Cloud::to_array_sdf(Vec2<int>   shape,
                          Vec4<float> bbox,
                          Array      *p_noise_x,
                          Array      *p_noise_y,
                          Vec4<float> bbox_array)
{
  // nodes
  std::vector<float> xp = this->get_x();
  std::vector<float> yp = this->get_y();

  for (size_t k = 0; k < xp.size(); k++)
  {
    xp[k] = (xp[k] - bbox.a) / (bbox.b - bbox.a);
    yp[k] = (yp[k] - bbox.c) / (bbox.d - bbox.c);
  }

  // fill heightmap
  auto distance_fct = [&xp, &yp](float x, float y, float)
  {
    float d = std::numeric_limits<float>::max();
    for (size_t i = 0; i < xp.size(); i++)
      d = std::min(d, std::hypot(x - xp[i], y - yp[i]));
    return std::sqrt(d);
  };

  Array z = Array(shape);
  fill_array_using_xy_function(z,
                               bbox_array,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               nullptr,
                               distance_fct);
  return z;
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

//----------------------------------------------------------------------
// functions
//----------------------------------------------------------------------

Cloud merge_cloud(Cloud &cloud1, Cloud &cloud2)
{
  std::vector<float> x1 = cloud1.get_x();
  std::vector<float> y1 = cloud1.get_y();
  std::vector<float> v1 = cloud1.get_values();

  std::vector<float> x2 = cloud2.get_x();
  std::vector<float> y2 = cloud2.get_y();
  std::vector<float> v2 = cloud2.get_values();

  x1.insert(x1.end(), x2.begin(), x2.end());
  y1.insert(y1.end(), y2.begin(), y2.end());
  v1.insert(v1.end(), v2.begin(), v2.end());

  return Cloud(x1, y1, v1);
}

} // namespace hmap
