/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
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

Cloud::Cloud(const std::vector<float> &x,
             const std::vector<float> &y,
             float                     default_value)
{
  if (x.size() != y.size())
    throw std::invalid_argument("x and y vectors must be of equal size");

  this->points.reserve(x.size());

  for (size_t k = 0; k < x.size(); ++k)
    this->points.emplace_back(x[k], y[k], default_value);
}

Cloud::Cloud(const std::vector<float> &x,
             const std::vector<float> &y,
             const std::vector<float> &v)
{
  if (x.size() != y.size() || x.size() != v.size())
    throw std::invalid_argument("x, y, and v vectors must be of equal size");

  this->points.reserve(x.size());

  for (size_t k = 0; k < x.size(); ++k)
    this->points.emplace_back(x[k], y[k], v[k]);
}

void Cloud::add_point(const Point &p)
{
  this->points.push_back(p);
}

void Cloud::clear()
{
  this->points.clear();
}

bool Cloud::from_csv(const std::string &fname)
{
  std::ifstream file(fname);
  if (!file.is_open())
  {
    LOG_ERROR("Could not open file: %s", fname.c_str());
    return false;
  }

  std::vector<Point> new_points;
  std::string        line;
  size_t             line_num = 0;
  const auto         old_locale = std::locale::global(std::locale("C"));

  while (std::getline(file, line))
  {
    ++line_num;
    if (line.empty()) continue;

    std::istringstream ss(line);
    std::vector<float> values;
    std::string        token;

    while (std::getline(ss, token, ','))
    {
      try
      {
        values.push_back(std::stof(token));
      }
      catch (const std::invalid_argument &)
      {
        LOG_ERROR("Invalid number format in CSV line %zu: '%s'",
                  line_num,
                  token.c_str());
        std::locale::global(old_locale);
        return false;
      }
    }

    if (values.size() == 2)
    {
      new_points.emplace_back(values[0], values[1], 0.0f);
    }
    else if (values.size() == 3)
    {
      new_points.emplace_back(values[0], values[1], values[2]);
    }
    else
    {
      LOG_ERROR("Invalid number of values (%zu) in CSV line %zu",
                values.size(),
                line_num);
      std::locale::global(old_locale);
      return false;
    }
  }

  std::locale::global(old_locale);
  this->points = std::move(new_points);
  return true;
}

Vec4<float> Cloud::get_bbox() const
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

Point Cloud::get_center() const
{
  Point center = Point();
  for (auto &p : this->points)
    center = center + p;
  return center / (float)this->points.size();
}

std::vector<int> Cloud::get_convex_hull_point_indices() const
{
  delaunator::Delaunator d(this->get_xy());

  std::vector<int> chull = {(int)d.hull_start};

  int inext = (int)d.hull_next[chull.back()];
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

float Cloud::get_values_max() const
{
  if (this->get_npoints() == 0) return 0.f;

  std::vector<float> values = this->get_values();
  return *std::max_element(values.begin(), values.end());
}

float Cloud::get_values_min() const
{
  if (this->get_npoints() == 0) return 0.f;

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
  const float     inv_width = 1.0f / (bbox.b - bbox.a);
  const float     inv_height = 1.0f / (bbox.d - bbox.c);
  const Vec2<int> shape = {array.shape.x - 1, array.shape.y - 1};

  std::vector<float> values;
  values.reserve(points.size());

  for (const auto &p : points)
  {
    const float xn = (p.x - bbox.a) * inv_width;
    const float yn = (p.y - bbox.c) * inv_height;

    if (xn < 0.0f || xn > 1.0f || yn < 0.0f || yn > 1.0f)
    {
      values.push_back(0.0f);
      continue;
    }

    const float x_scaled = xn * shape.x;
    const float y_scaled = yn * shape.y;
    const int   i = static_cast<int>(x_scaled);
    const int   j = static_cast<int>(y_scaled);

    if (i >= 0 && i < array.shape.x && j >= 0 && j < array.shape.y)
    {
      const float uu = x_scaled - i;
      const float vv = y_scaled - j;
      values.push_back(array.get_value_bilinear_at(i, j, uu, vv));
    }
    else
    {
      values.push_back(0.0f);
    }
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
  std::uniform_real_distribution<float> x_dist(bbox.a, bbox.b);
  std::uniform_real_distribution<float> y_dist(bbox.c, bbox.d);
  std::uniform_real_distribution<float> v_dist(0.0f, 1.0f);

  for (auto &p : this->points)
  {
    p.x = x_dist(gen);
    p.y = y_dist(gen);
    p.v = v_dist(gen);
  }
}

void Cloud::remap_values(float vmin, float vmax)
{
  const auto [current_min, current_max] = std::minmax_element(
      this->points.begin(),
      this->points.end(),
      [](const Point &a, const Point &b) { return a.v < b.v; });

  if (current_min->v == current_max->v) return;

  const float scale = (vmax - vmin) / (current_max->v - current_min->v);
  for (auto &p : this->points)
    p.v = (p.v - current_min->v) * scale + vmin;
}

void Cloud::remove_point(int point_idx)
{
  this->points.erase(this->points.begin() + point_idx);
}

void Cloud::set_values(const std::vector<float> &new_values)
{
  if (new_values.size() != this->points.size())
    throw std::invalid_argument("New values size must match number of points");

  for (size_t k = 0; k < this->points.size(); ++k)
    this->points[k].v = new_values[k];
}

void Cloud::set_values(float new_value)
{
  for (size_t k = 0; k < this->points.size(); ++k)
    this->points[k].v = new_value;
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

void Cloud::to_array(Array &array, Vec4<float> bbox) const
{
  int   ni = array.shape.x;
  int   nj = array.shape.y;
  float ai = (ni - 1) / (bbox.b - bbox.a);
  float bi = -bbox.a * (ni - 1) / (bbox.b - bbox.a);
  float aj = (nj - 1) / (bbox.d - bbox.c);
  float bj = -bbox.c * (nj - 1) / (bbox.d - bbox.c);

  for (auto &p : this->points)
  {
    const int i = static_cast<int>(std::round(ai * p.x + bi));
    const int j = static_cast<int>(std::round(aj * p.y + bj));

    if ((i > -1) and (i < ni) and (j > -1) and (j < nj)) array(i, j) = p.v;
  }
}

void Cloud::to_array_interp(Array                &array,
                            Vec4<float>           bbox,
                            InterpolationMethod2D interpolation_method,
                            Array                *p_noise_x,
                            Array                *p_noise_y,
                            Vec4<float>           bbox_array) const
{
  std::vector<float> x = this->get_x();
  std::vector<float> y = this->get_y();
  std::vector<float> v = this->get_values();

  const float       lx = bbox.b - bbox.a;
  const float       ly = bbox.d - bbox.c;
  const Vec4<float> bbox_expanded = {bbox.a - lx,
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
                          Vec4<float> bbox_array) const
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

void Cloud::to_csv(const std::string &fname) const
{
  std::ofstream f(fname, std::ios::out);

  if (!f.is_open()) throw std::runtime_error("Failed to open file: " + fname);

  // Use C locale for consistent number formatting
  f.imbue(std::locale("C"));
  f << std::fixed << std::setprecision(9);

  for (const auto &p : this->points)
    f << p.x << ',' << p.y << ',' << p.v << '\n';
}

Graph Cloud::to_graph_delaunay()
{
  delaunator::Delaunator d(this->get_xy());
  Graph                  graph = Graph(*this);

  for (std::size_t e = 0; e < d.triangles.size(); e++)
  {
    int i = (int)d.halfedges[e];
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
