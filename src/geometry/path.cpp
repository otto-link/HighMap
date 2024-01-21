/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <limits>
#include <list>
#include <random>

#include "BSpline.h"
#include "Bezier.h"
#include "CatmullRom.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void Path::bezier(float curvature_ratio, int edge_divisions)
{
  Path new_path = Path();

  size_t ks = this->closed ? 0 : 1;
  for (size_t k = 0; k < this->get_npoints() - ks; k++)
  {
    size_t kp1 = (k + 1) % this->get_npoints();
    size_t kp2 = (k + 2) % this->get_npoints();

    Bezier curve = Bezier();
    curve.set_steps(edge_divisions);

    curve.add_way_point(Vector(this->points[k].x, this->points[k].y, 0.f));

    {
      Point p = lerp(this->points[k], this->points[kp1], curvature_ratio);
      curve.add_way_point(Vector(p.x, p.y, 0.f));
    }

    if (!this->closed && k == this->get_npoints() - 2)
    {
      Point p = lerp(this->points[k], this->points[kp1], curvature_ratio);
      curve.add_way_point(Vector(p.x, p.y, 0.f));
    }
    else
    {
      Point p = lerp(this->points[kp1], this->points[kp2], -curvature_ratio);
      curve.add_way_point(Vector(p.x, p.y, 0.f));
    }

    curve.add_way_point(Vector(this->points[kp1].x, this->points[kp1].y, 0.f));

    for (int i = 0; i < curve.node_count(); ++i)
    {
      // interpolate value
      float s = curve.length_from_starting_point(i) / curve.total_length();
      float v = (1. - s) * this->points[k].v + s * this->points[kp1].v;

      new_path.add_point(Point(curve.node(i).x, curve.node(i).y, v));
    }
  }

  *this = new_path;
}

void Path::bezier_round(float curvature_ratio, int edge_divisions)
{
  Path new_path = Path();

  size_t ks = this->closed ? 0 : 1;
  for (size_t k = 0; k < this->get_npoints() - ks; k++)
  {
    size_t km1 = (k - 1) % this->get_npoints();
    size_t kp1 = (k + 1) % this->get_npoints();
    size_t kp2 = (k + 2) % this->get_npoints();

    Bezier curve = Bezier();
    curve.set_steps(edge_divisions);

    curve.add_way_point(Vector(this->points[k].x, this->points[k].y, 0.f));

    if (!this->closed && k == 0)
    {
      Point p = lerp(this->points[k], this->points[kp1], curvature_ratio);
      curve.add_way_point(Vector(p.x, p.y, 0.f));
    }
    else
    {
      float dx = this->points[kp1].x - this->points[km1].x;
      float dy = this->points[kp1].y - this->points[km1].y;
      Point p = Point(this->points[k].x + curvature_ratio * dx,
                      this->points[k].y + curvature_ratio * dy,
                      0.f);
      curve.add_way_point(Vector(p.x, p.y, 0.f));
    }

    if (!this->closed && k == this->get_npoints() - 1)
    {
      Point p = lerp(this->points[kp1], this->points[kp2], curvature_ratio);
      curve.add_way_point(Vector(p.x, p.y, 0.f));
    }
    else
    {
      float dx = this->points[kp2].x - this->points[k].x;
      float dy = this->points[kp2].y - this->points[k].y;
      Point p = Point(this->points[kp1].x - curvature_ratio * dx,
                      this->points[kp1].y - curvature_ratio * dy,
                      0.f);
      curve.add_way_point(Vector(p.x, p.y, 0.f));
    }

    curve.add_way_point(Vector(this->points[kp1].x, this->points[kp1].y, 0.f));

    for (int i = 0; i < curve.node_count(); ++i)
    {
      // interpolate value
      float s = curve.length_from_starting_point(i) / curve.total_length();
      float v = (1. - s) * this->points[k].v + s * this->points[kp1].v;

      new_path.add_point(Point(curve.node(i).x, curve.node(i).y, v));
    }
  }

  *this = new_path;
}

void Path::bspline(int edge_divisions)
{
  Path new_path = Path();

  BSpline curve = BSpline();
  curve.set_steps(edge_divisions * (int)this->get_npoints());

  size_t ks = this->closed ? 1 : 0;
  for (size_t k = 0; k < this->get_npoints() + ks; k++)
  {
    if (k == 0)
      curve.add_way_point(
          Vector(this->points[0].x, this->points[0].y, this->points[0].v));

    size_t k0 = k % this->get_npoints();
    curve.add_way_point(
        Vector(this->points[k0].x, this->points[k0].y, this->points[k0].v));

    if (k == (this->get_npoints() + ks - 1))
      curve.add_way_point(
          Vector(this->points[k0].x, this->points[k0].y, this->points[k0].v));
  }

  for (int i = 0; i < curve.node_count(); ++i)
    new_path.add_point(
        Point(curve.node(i).x, curve.node(i).y, curve.node(i).z));

  *this = new_path;
}

void Path::catmullrom(int edge_divisions)
{
  Path new_path = Path();

  CatmullRom curve = CatmullRom();
  curve.set_steps(edge_divisions * (int)this->get_npoints());

  size_t ks = this->closed ? 1 : 0;
  for (size_t k = 0; k < this->get_npoints() + ks; k++)
  {
    if (k == 0)
      curve.add_way_point(
          Vector(this->points[0].x, this->points[0].y, this->points[0].v));

    size_t k0 = k % this->get_npoints();
    curve.add_way_point(
        Vector(this->points[k0].x, this->points[k0].y, this->points[k0].v));

    if (k == (this->get_npoints() + ks - 1))
      curve.add_way_point(
          Vector(this->points[k0].x, this->points[k0].y, this->points[k0].v));
  }

  for (int i = 0; i < curve.node_count(); ++i)
    new_path.add_point(
        Point(curve.node(i).x, curve.node(i).y, curve.node(i).z));

  *this = new_path;
}

void Path::clear()
{
  this->points.clear();
  this->convex_hull.clear();
  this->closed = false;
}

void Path::dijkstra(Array      &array,
                    Vec4<float> bbox,
                    int         edge_divisions,
                    float       elevation_ratio,
                    float       distance_exponent,
                    Array      *p_mask_nogo)
{
  size_t ks = this->closed ? 0 : 1; // trick to handle closed contours
  for (size_t k = 0; k < this->get_npoints() - ks; k++)
  {
    size_t knext = (k + 1) % this->get_npoints();

    Vec2<int> ij_start = Vec2<int>(
        (int)((this->points[k].x - bbox.a) / (bbox.b - bbox.a) *
              (array.shape.x - 1)),
        (int)((this->points[k].y - bbox.c) / (bbox.d - bbox.c) *
              (array.shape.y - 1)));

    Vec2<int> ij_end = Vec2<int>(
        (int)((this->points[knext].x - bbox.a) / (bbox.b - bbox.a) *
              (array.shape.x - 1)),
        (int)((this->points[knext].y - bbox.c) / (bbox.d - bbox.c) *
              (array.shape.y - 1)));

    float dist_idx = std::hypot((float)(ij_start.x - ij_end.x),
                                (float)(ij_start.y - ij_end.y));

    Vec2<int> step;
    if (edge_divisions > 0)
    {
      int div = std::max(1, (int)(dist_idx / edge_divisions));
      step = Vec2<int>(div, div);
    }
    else
      step = Vec2<int>(1, 1);

    std::vector<int> ip, jp;
    array.find_path_dijkstra(ij_start,
                             ij_end,
                             ip,
                             jp,
                             elevation_ratio,
                             distance_exponent,
                             step,
                             p_mask_nogo);

    // backup cuurrent edge informations before adding points to this edge
    Point p1 = this->points[k];
    Point p2 = this->points[knext];

    for (size_t r = 1; r < ip.size() - 1; r++)
    {
      float x = (float)ip[r] / (float)(array.shape.x - 1) * (bbox.b - bbox.a) +
                bbox.a;
      float y = (float)jp[r] / (float)(array.shape.y - 1) * (bbox.d - bbox.c) +
                bbox.c;

      // use distance to start and end points to determine value at the added
      // point (barycentric value)
      float d1 = std::hypot(x - p1.x, y - p1.y);
      float d2 = std::hypot(x - p2.x, y - p2.y);
      float v = (d2 * p1.v + d1 * p2.v) / (d1 + d2);

      Point p = Point(x, y, v);
      this->points.insert(this->points.begin() + k + 1, p);
      ++k;
    }
  }
}

void Path::divide()
{
  size_t ks = this->closed ? 0 : 1; // trick to handle closed contours
  for (size_t k = 0; k < this->get_npoints() - ks; k++)
  {
    size_t knext = (k + 1) % this->get_npoints();
    Point  p = lerp(this->points[k], this->points[knext], 0.5f);
    this->points.insert(this->points.begin() + k + 1, p);
    ++k;
  }
}

void Path::fractalize(int   iterations,
                      uint  seed,
                      float sigma,
                      int   orientation,
                      float persistence)
{
  std::mt19937                    gen(seed);
  std::normal_distribution<float> dis(0.f, sigma);

  for (int it = 0; it < iterations; it++)
  {
    // add a mid point between each points and shuffle the position of
    // this new point
    this->divide();

    size_t ks = this->closed ? 0 : 1;
    for (size_t k = 1; k < this->get_npoints() - ks; k += 2)
    {
      size_t knext = (k + 1) % this->get_npoints();

      float amp = dis(gen);
      float alpha = angle(this->points[k - 1], this->points[knext]) + M_PI_2;
      float dist = distance(this->points[k - 1], this->points[knext]);

      if (orientation != 0)
        amp = std::abs(amp) * (float)std::copysign(1, orientation);

      this->points[k].x += amp * dist * std::cos(alpha);
      this->points[k].y += amp * dist * std::sin(alpha);
    }
    sigma *= persistence;
  }
}

std::vector<float> Path::get_arc_length()
{
  std::vector<float> s = this->get_cumulative_distance();
  // normalize in [0, 1]
  for (auto &v : s)
    v /= s.back();
  return s;
}

std::vector<float> Path::get_cumulative_distance()
{
  size_t             ke = this->closed ? 1 : 0;
  std::vector<float> dacc(this->get_npoints() + ke);

  for (size_t k = 1; k < this->get_npoints() + ke; k++)
  {
    size_t knext = (k + 1) % this->get_npoints();
    float  dist = distance(this->points[k - 1], this->points[knext]);
    dacc[k] = dacc[k - 1] + dist;
  }

  return dacc;
}

void Path::meanderize(float ratio, int iterations, int edge_divisions)
{
  for (int it = 0; it < iterations; it++)
  {
    Path new_path = Path();

    size_t ks = this->closed ? 0 : 1;
    for (size_t k = 0; k < this->get_npoints() - ks; k++)
    {
      size_t kp1 = (k + 1) % this->get_npoints();
      size_t kp2 = (k + 2) % this->get_npoints();

      new_path.add_point(
          Point(this->points[k].x, this->points[k].y, this->points[k].v));

      float alpha = angle(this->points[kp1], this->points[k]);

      float dist = distance(this->points[kp1], this->points[k]);
      Point p = lerp(this->points[k], this->points[kp1], 0.5f);

      // vector relative orientation based on cross-product
      float cross_product = (this->points[kp2].y - this->points[k].y) *
                                (this->points[kp1].x - this->points[k].x) -
                            (this->points[kp2].x - this->points[k].x) *
                                (this->points[kp1].y - this->points[k].y);

      if (cross_product >= 0.f)
        alpha += M_PI_2;
      else
        alpha -= M_PI_2;

      p.x += ratio * dist * std::cos(alpha);
      p.y += ratio * dist * std::sin(alpha);

      new_path.add_point(p);
    }

    if (this->closed)
      new_path.add_point(this->points[0]);
    else
      new_path.add_point(this->points.back());

    *this = new_path;
  }

  this->bspline(edge_divisions);
}

void Path::reorder_nns(int start_index)
{
  // new path indices
  std::vector<int> idx = {start_index};

  // brute force nearest neighbor search...
  std::list<int> queue_search = {};
  for (size_t k = 0; k < this->get_npoints(); k++)
    if ((int)k != start_index)
      queue_search.push_back((int)k);

  while (idx.size() < this->get_npoints())
  {
    int   k = idx.back(); // current point
    int   knext = 0;      // what we are looking: the next point
    float dmin = std::numeric_limits<float>::max();

    for (auto &i : queue_search)
    {
      if (i != k)
      {
        float dist = distance(this->points[k], this->points[i]);
        if (dist < dmin)
        {
          dmin = dist;
          knext = i;
        }
      }
    }
    queue_search.remove(knext);
    idx.push_back(knext);
  }

  // new set of reordered points
  std::vector<Point> points = {};
  for (size_t k = 0; k < this->get_npoints(); k++)
    points.push_back(this->points[idx[k]]);

  this->points = points;
}

void Path::resample(float delta)
{
  // redivide each edge
  size_t ks = this->closed ? 0 : 1;

  for (size_t k = 0; k < this->get_npoints() - ks; k++)
  {
    size_t knext = (k + 1) % this->get_npoints();
    float  dist = distance(this->points[k], this->points[knext]);
    int    ndiv = (int)(dist / delta);
    Point  p1 = this->points[k];
    Point  p2 = this->points[knext];

    if (ndiv > 1)
    {
      for (int i = 1; i < ndiv; i++)
      {
        float r = (float)i / (float)ndiv;
        Point p = lerp(p1, p2, r);
        this->points.insert(this->points.begin() + k + i, p);
      }
      k += ndiv - 1;
    }
  }
}

void Path::resample_uniform()
{
  // determine smallest distance between two consecutive points (and
  // store distances because there are used for the interpolation
  // step)
  float dmin = std::numeric_limits<float>::max();

  size_t ks = this->closed ? 0 : 1;
  for (size_t k = 0; k < this->get_npoints() - ks; k++)
  {
    size_t knext = (k + 1) % this->get_npoints();
    float  dist = distance(this->points[k], this->points[knext]);
    if (dist < dmin)
      dmin = dist;
  }

  this->resample(dmin);
}

void Path::reverse()
{
  std::reverse(this->points.begin(), this->points.end());
}

void Path::subsample(int step)
{
  size_t k_global = 0;
  size_t k_last = this->get_npoints() - 1; // to keep the end point

  for (size_t k = 0; k < this->get_npoints(); k++)
  {
    if ((k_global % step != 0) and (k_global != k_last))
    {
      this->points.erase(this->points.begin() + k);
      k--;
    }
    k_global++;
  }
}

void Path::to_array(Array &array, Vec4<float> bbox, bool filled)
{
  // number of pixels per unit length
  float lx = bbox.b - bbox.a;
  float ly = bbox.d - bbox.c;
  float ppu = std::max(array.shape.x / lx, array.shape.y / ly);

  // create a temporary cloud with the right points density (= 1 ppu)
  Cloud cloud = Cloud(points);

  // project path itself
  size_t ks = this->closed ? 0 : 1;
  for (size_t k = 0; k < this->get_npoints() - ks; k++)
  {
    size_t knext = (k + 1) % this->get_npoints();
    int    npixels = (int)std::ceil(
                      distance(this->points[k], this->points[knext]) * ppu) +
                  1;

    for (int i = 0; i < npixels; i++)
    {
      float t = (float)i / (float)(npixels - 1);
      Point p = lerp(this->points[k], this->points[knext], t);
      cloud.add_point(p);
    }
  }

  // if filled, set the border to the same value of the filling value
  if (filled)
    cloud.set_values(1.f);

  cloud.to_array(array, bbox);

  // flood filling
  if (filled)
  {
    Array array_bckp = array;

    // TODO make something more robust
    int i = 0;
    int j = 0;

    flood_fill(array, i, j);

    array = 1.f - array;
    array = maximum(array, array_bckp);
  }
}

Array Path::to_array_gaussian(Vec2<int>   shape,
                              Vec4<float> bbox,
                              float       width,
                              Array      *p_noise_x,
                              Array      *p_noise_y,
                              Vec2<float> shift,
                              Vec2<float> scale)
{
  std::vector<float> x = this->get_x();
  std::vector<float> y = this->get_y();

  for (size_t k = 0; k < x.size(); k++)
  {
    x[k] = (x[k] - bbox.a) / (bbox.b - bbox.a);
    y[k] = (y[k] - bbox.c) / (bbox.d - bbox.c);
  }

  Array z = -sdf_path(shape, x, y, p_noise_x, p_noise_y, shift, scale);

  z = exp(-0.5f * z * z / (width * width));

  return z;
}

Array Path::to_array_polygon(Vec2<int>   shape,
                             Vec4<float> bbox,
                             Array      *p_noise_x,
                             Array      *p_noise_y,
                             Vec2<float> shift,
                             Vec2<float> scale)
{
  std::vector<float> x = this->get_x();
  std::vector<float> y = this->get_y();

  for (size_t k = 0; k < x.size(); k++)
  {
    x[k] = (x[k] - bbox.a) / (bbox.b - bbox.a);
    y[k] = (y[k] - bbox.c) / (bbox.d - bbox.c);
  }

  Array z = -sdf_polygon(shape, x, y, p_noise_x, p_noise_y, shift, scale);
  return z;
}

Array Path::to_array_range(Vec2<int>   shape,
                           Vec4<float> bbox,
                           float       aspect_ratio,
                           Array      *p_noise_x,
                           Array      *p_noise_y,
                           Vec2<float> shift,
                           Vec2<float> scale)
{
  // build up a path encompassing the original path, distance between
  // the points of this "hull" path and the points of the input path
  // is driven by the value at the points.
  Path path_hull = Path();

  if (this->get_npoints() > 0)
  {

    // new points are orthognal to the path
    Path ph1 = Path();
    Path ph2 = Path();

    // but start point is in the alignment of the path
    {
      int   k = 0;
      float alpha = angle(this->points[k + 1], this->points[k]);
      float dx = aspect_ratio * this->points[k].v * std::cos(alpha);
      float dy = aspect_ratio * this->points[k].v * std::sin(alpha);
      ph1.add_point(Point(this->points[k].x + dx, this->points[k].y + dy, 1.f));
    }

    for (size_t k = 0; k < this->get_npoints(); k++)
    {
      float alpha;
      if (k == 0)
        alpha = angle(this->points[k + 1], this->points[k]) + M_PI_2;
      else if (k == this->get_npoints() - 1)
        alpha = angle(this->points[k], this->points[k - 1]) + M_PI_2;
      else
        alpha = angle(this->points[k + 1], this->points[k - 1]) + M_PI_2;

      float dx = aspect_ratio * this->points[k].v * std::cos(alpha);
      float dy = aspect_ratio * this->points[k].v * std::sin(alpha);

      ph1.add_point(Point(this->points[k].x + dx, this->points[k].y + dy, 1.f));
      ph2.add_point(
          Point(this->points[k].x - dx, this->points[k].y - dy, 0.7f));
    }

    // add end points in the alignment of the path
    {
      int   k = this->get_npoints() - 1;
      float alpha = angle(this->points[k - 1], this->points[k]);
      float dx = aspect_ratio * this->points[k].v * std::cos(alpha);
      float dy = aspect_ratio * this->points[k].v * std::sin(alpha);
      ph2.add_point(Point(this->points[k].x + dx, this->points[k].y + dy, 1.f));
    }

    // concatenate everything
    for (size_t k = 0; k < ph1.get_npoints(); k++)
      path_hull.add_point(ph1.points[k]);

    for (size_t k = 0; k < ph2.get_npoints(); k++)
      path_hull.add_point(ph2.points[ph2.get_npoints() - 1 - k]);
  }

  Array z = Array(shape);

  path_hull.to_array(z, bbox);

  z = path_hull
          .to_array_polygon(shape, bbox, p_noise_x, p_noise_y, shift, scale);

  return z;
}

void Path::to_png(std::string fname, Vec2<int> shape)
{
  Array array = Array(shape);
  this->to_array(array, this->get_bbox());
  array.to_png(fname, cmap::inferno, false);
}

//----------------------------------------------------------------------
// functions
//----------------------------------------------------------------------

void dig_path(Array      &z,
              Path       &path,
              int         width,
              int         decay,
              int         flattening_radius,
              bool        force_downhill,
              Vec4<float> bbox,
              float       depth)
{
  Array mask = Array(z.shape);
  Path  path_copy = path;
  Array zf;

  if (force_downhill)
  {
    // make sure the path is monotically decreasing
    path_copy.set_values_from_array(z, bbox);

    for (size_t k = 1; k < path_copy.get_npoints(); k++)
      if (path_copy.points[k].v > path_copy.points[k - 1].v)
        path_copy.points[k].v = path_copy.points[k - 1].v;

    path_copy.to_array(mask, bbox);
    zf = maximum_local(mask, 3 * (width + decay));
    smooth_cpulse(zf, width + decay);

    // regenerate the mask
    path_copy.set_values(1.f);
    path_copy.to_array(mask, bbox);
  }
  else
  {
    // make sure values at the path points are non-zero before creating
    // the mask
    Path path_copy = path;
    path_copy.set_values(1.f);

    path_copy.to_array(mask, bbox);
    zf = mean_local(z, flattening_radius);
  }

  mask = maximum_local(mask, width);
  mask = distance_transform(mask);
  mask = exp(-mask * mask * 0.5f / ((float)(decay * decay)));
  smooth_cpulse(mask, decay);

  zf += depth;
  z = lerp(z, zf, mask);
}

} // namespace hmap
