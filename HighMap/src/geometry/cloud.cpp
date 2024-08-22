/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <fstream>
#include <iomanip>
#include <string>

#include "delaunator-cpp.hpp"
#include "macrologger.h"

#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/graph.hpp"
#include "highmap/geometry/grids.hpp"
#include "highmap/interpolate2d.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

Cloud::Cloud(int npoints, uint seed, Vec4<float> bbox)
{
  this->points.resize(npoints);
  this->randomize(seed, bbox);
};

Cloud::Cloud(std::vector<float> x, std::vector<float> y, float default_value)
{
  for (size_t k = 0; k < x.size(); k++)
    this->add_point(Point(x[k], y[k], default_value));
};

Cloud::Cloud(std::vector<float> x, std::vector<float> y, std::vector<float> v)
{
  for (size_t k = 0; k < x.size(); k++)
    this->add_point(Point(x[k], y[k], v[k]));
};

void Cloud::add_point(const Point &p)
{
  this->points.push_back(p);
}

void Cloud::clear()
{
  this->points.clear();
}

Vec4<float> Cloud::get_bbox()
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

Point Cloud::get_center()
{
  Point center = Point();
  for (auto &p : this->points)
    center = center + p;
  return center / (float)this->points.size();
}

std::vector<int> Cloud::get_convex_hull_point_indices()
{
  delaunator::Delaunator d(this->get_xy());

  std::vector<int> chull = {(int)d.hull_start};

  int inext = d.hull_next[chull.back()];
  while (inext != chull[0])
  {
    chull.push_back(inext);
    inext = d.hull_next[chull.back()];
  }

  return chull;
}

size_t Cloud::get_npoints() const
{
  return this->points.size();
}

std::vector<float> Cloud::get_values() const
{
  std::vector<float> values;
  values.reserve(this->get_npoints());
  for (auto &p : this->points)
    values.push_back(p.v);
  return values;
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

std::vector<float> Cloud::get_x() const
{
  std::vector<float> x;
  x.reserve(this->get_npoints());
  for (auto &p : this->points)
    x.push_back(p.x);
  return x;
}

std::vector<float> Cloud::get_xy() const
{
  std::vector<float> xy;
  xy.reserve(2 * this->get_npoints());
  for (auto &p : this->points)
  {
    xy.push_back(p.x);
    xy.push_back(p.y);
  }
  return xy;
}

std::vector<float> Cloud::get_y() const
{
  std::vector<float> y;
  y.reserve(this->get_npoints());
  for (auto &p : this->points)
    y.push_back(p.y);
  return y;
}

std::vector<float> Cloud::interpolate_values_from_array(const Array &array,
                                                        Vec4<float>  bbox)
{
  std::vector<float> values;
  values.reserve(this->get_npoints());

  for (auto &p : points)
  {
    float xn = (p.x - bbox.a) / (bbox.b - bbox.a);
    float yn = (p.y - bbox.c) / (bbox.d - bbox.c);

    // scale to array shape
    xn *= (float)(array.shape.x - 1);
    yn *= (float)(array.shape.y - 1);

    int i = (int)xn;
    int j = (int)yn;

    if (i >= 0 && i < array.shape.x && j >= 0 && j < array.shape.y)
    {
      float uu = xn - (float)i;
      float vv = yn - (float)j;
      values.push_back(array.get_value_bilinear_at(i, j, uu, vv));
    }
    else
      values.push_back(0.f); // if outside array bounding box
  }

  return values;
}

void Cloud::print()
{
  std::cout << "Cloud" << std::endl;

  Vec4<float> bbox = this->get_bbox();
  Point       center = this->get_center();

  std::cout << "  - bounding box: {" << bbox.a << ", " << bbox.b << ", "
            << bbox.c << ", " << bbox.d << "}" << std::endl;

  std::cout << "  - center: {" << center.x << ", " << center.y << "}"
            << std::endl;

  std::cout << "  - points:" << std::endl;
  for (size_t k = 0; k < this->get_npoints(); k++)
  {
    std::cout << std::setw(6) << k;
    std::cout << std::setw(12) << this->points[k].x;
    std::cout << std::setw(12) << this->points[k].y;
    std::cout << std::setw(12) << this->points[k].v;
    std::cout << std::endl;
  }
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

void Cloud::remove_point(int point_idx)
{
  this->points.erase(this->points.begin() + point_idx);
}

void Cloud::set_values(std::vector<float> new_values)
{
  for (size_t k = 0; k < this->get_npoints(); k++)
    this->points[k].v = new_values[k];
}

void Cloud::set_values(float new_value)
{
  for (auto &p : this->points)
    p.v = new_value;
}

void Cloud::set_values_from_array(const Array &array, Vec4<float> bbox)
{
  for (auto &p : this->points)
    p.update_value_from_array(array, bbox);
}

void Cloud::set_values_from_chull_distance()
{
  std::vector<int> chull = this->get_convex_hull_point_indices();

  for (size_t i = 0; i < this->get_npoints(); i++)
  {
    float dmax = std::numeric_limits<float>::max();
    for (size_t k = 0; k < chull.size(); k++)
    {
      float dist = distance(this->points[i], this->points[chull[k]]);
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

    if ((i > -1) and (i < ni) and (j > -1) and (j < nj)) array(i, j) = p.v;
  }
}

void Cloud::to_array_interp(Array                &array,
                            Vec4<float>           bbox,
                            InterpolationMethod2D interpolation_method,
                            Array                *p_noise_x,
                            Array                *p_noise_y,
                            Vec4<float>           bbox_array)
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

  array = interpolate2d(array.shape,
                        x,
                        y,
                        v,
                        interpolation_method,
                        p_noise_x,
                        p_noise_y,
                        nullptr,
                        bbox_array);
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
  delaunator::Delaunator d(this->get_xy());
  Graph                  graph = Graph(*this);

  for (std::size_t e = 0; e < d.triangles.size(); e++)
  {
    int i = d.halfedges[e];
    if (((int)e > i) or (i == -1))
    {
      int next_he = (e % 3 == 2) ? e - 2 : e + 1;
      graph.add_edge({(int)d.triangles[e], (int)d.triangles[next_he]});
    }
  }

  return graph;
}

void Cloud::to_png(const std::string &fname,
                   int                cmap,
                   Vec4<float>        bbox,
                   int                depth,
                   Vec2<int>          shape)
{
  Array array = Array(shape);
  this->to_array(array, bbox);
  array.to_png(fname, cmap, depth);
}

Cloud merge_cloud(const Cloud &cloud1, const Cloud &cloud2)
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
