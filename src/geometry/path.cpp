/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#define _USE_MATH_DEFINES

#include <cmath>
#include <limits>
#include <list>
#include <random>

#include "bezier.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void Path::bezier(float curvature_ratio, int edge_divisions)
{
  // backup 2nd point for closed path
  Point p_bckp = this->points[1];

  size_t ks = this->closed ? 0 : 1; // trick to handle closed contours
  for (size_t k = 0; k < this->get_npoints() - ks; k++)
  {
    size_t kp1 = (k + 1) % this->get_npoints();

    // start point
    Bezier::Point p1 = Bezier::Point(this->points[k].x, this->points[k].y);

    // control #1
    float x2 = (1.f - curvature_ratio) * this->points[k].x +
               curvature_ratio * this->points[kp1].x;
    float y2 = (1.f - curvature_ratio) * this->points[k].y +
               curvature_ratio * this->points[kp1].y;

    Bezier::Point p2 = Bezier::Point(x2, y2);

    // control #2
    Bezier::Point p3 = Bezier::Point();

    if ((k == this->get_npoints() - ks - 1) and (this->closed == false))
      p3 = p2;
    else
    {
      size_t kp2 = (k + 2) % this->get_npoints();

      Point point_kp2 = Point();
      if (kp2 == 1)
        point_kp2 = p_bckp;
      else
        point_kp2 = this->points[kp2];

      float x3 = 2.f * this->points[kp1].x -
                 (1.f - curvature_ratio) * this->points[kp1].x -
                 curvature_ratio * point_kp2.x;
      float y3 = 2.f * this->points[kp1].y -
                 (1.f - curvature_ratio) * this->points[kp1].y -
                 curvature_ratio * point_kp2.y;

      p3 = Bezier::Point(x3, y3);
    }

    // end point
    Bezier::Point p4 = Bezier::Point(this->points[kp1].x, this->points[kp1].y);

    // Bezier curve
    std::vector<Bezier::Point> xy = {p1, p2, p3, p4};
    Bezier::Bezier<3>          cubicBezier(xy);

    for (int i = 1; i < edge_divisions - 1; i++)
    {
      float         s = (float)i / (float)(edge_divisions - 1);
      Bezier::Point p = cubicBezier.valueAt(s);

      float v = (1. - s) * this->points[k].v + s * this->points[kp1].v;
      Point pc = Point(p.x, p.y, v);

      this->points.insert(this->points.begin() + k + 1, pc);
      ++k;
    }
  }
}

void Path::dijkstra(Array      &array,
                    Vec4<float> bbox,
                    int         edge_divisions,
                    float       elevation_ratio,
                    float       distance_exponent)
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
                             step);

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

void Path::meanderize(float radius,
                      float tangent_contribution,
                      int   iterations,
                      float transition_length_ratio)
{
  float sigma = 0.5f;
  int   laplace_iterations = 3;

  for (int it = 0; it < iterations; it++)
  {
    std::vector<float> s = this->get_cumulative_distance();
    std::vector<float> ds = gradient1d(s);

    std::vector<float> tx = gradient1d(this->get_x());
    std::vector<float> ty = gradient1d(this->get_y());
    laplace1d(tx, sigma, laplace_iterations);
    laplace1d(ty, sigma, laplace_iterations);

    std::vector<float> nx = gradient1d(tx);
    std::vector<float> ny = gradient1d(ty);
    laplace1d(nx, sigma, laplace_iterations);
    laplace1d(ny, sigma, laplace_iterations);

    // scaling of the normal vector
    float scale_n = 0.f;
    for (size_t i = 0; i < this->get_npoints(); i++)
    {
      float d = std::hypot(nx[i], ny[i]);
      if (d > scale_n)
        scale_n = d;
    }
    scale_n = 1.f / scale_n;

    for (size_t i = 0; i < this->get_npoints(); i++)
    {
      // normalize
      float scale_t = 1.f / ds[i];
      tx[i] *= scale_t;
      ty[i] *= scale_t;

      nx[i] *= scale_n;
      ny[i] *= scale_n;

      float cx = tangent_contribution * tx[i] -
                 (1.f - tangent_contribution) * nx[i];
      float cy = tangent_contribution * ty[i] -
                 (1.f - tangent_contribution) * ny[i];

      // apply a shape factor to preserve starting and ending parts of
      // the curve
      float shape_factor = 1.f;
      float smax = s.back();
      if (s[i] < smax * transition_length_ratio)
        shape_factor = s[i] / (smax * transition_length_ratio);
      else if (s[i] > smax * (1.f - transition_length_ratio))
        shape_factor = 1.f - (s[i] / smax - 1.f + transition_length_ratio) /
                                 transition_length_ratio;

      // smooth transitions
      shape_factor = shape_factor * shape_factor * (3.f - 2.f * shape_factor);

      this->points[i].x += cx * radius * shape_factor;
      this->points[i].y += cy * radius * shape_factor;
    }

    // remesh to maintain the same distance between points
    this->resample(s.back() / (float)(this->get_npoints() - 1));
  }
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

void Path::to_array(Array &array, Vec4<float> bbox)
{
  // number of pixels per unit length
  float lx = bbox.b - bbox.a;
  float ly = bbox.d - bbox.c;
  float ppu = std::max(array.shape.x / lx, array.shape.y / ly);

  // create a temporary cloud with the right points density (= 1 ppu)
  Cloud cloud = Cloud(points);

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

  cloud.to_array(array, bbox);
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
              Vec4<float> bbox)
{
  Array mask = Array(z.shape);
  path.to_array(mask, bbox);

  mask = maximum_local(mask, width);
  mask = distance_transform(mask);
  mask = exp(-mask * mask * 0.5f / ((float)(decay * decay)));

  Array zf = mean_local(z, flattening_radius);

  z = lerp(z, zf, mask);
}

} // namespace hmap
