/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <limits>
#include <list>
#include <random>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/filters.hpp"
#include "highmap/geometry/path.hpp"
#include "highmap/internal/vector_utils.hpp"
#include "highmap/interpolate_curve.hpp"
#include "highmap/morphology.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"
#include "highmap/shortest_path.hpp"

namespace hmap
{

void Path::bezier(float curvature_ratio, int edge_divisions)
{
  // --- generate a new set of points by adding control points
  // --- inbetween path points

  std::vector<Point> new_points = {};
  new_points.reserve(this->get_npoints() + 2 * (this->get_npoints() - 1));

  size_t npoints = this->get_npoints();
  size_t end = this->closed ? npoints : npoints - 1;

  for (size_t k = 0; k < end; k++)
  {
    size_t knext = (k + 1) % npoints;
    size_t knext_after = (k + 2) % npoints;

    Point pc1 = lerp(this->points[k], this->points[knext], curvature_ratio);
    Point pc2 = lerp(this->points[knext],
                     this->points[knext_after],
                     -curvature_ratio);

    new_points.push_back(this->points[k]);
    new_points.push_back(pc1);
    new_points.push_back(pc2);
  }

  // add **first** point to close the loop
  if (this->closed) new_points.push_back(this->points.front());

  // --- interpolate

  InterpolatorCurve  fitp = InterpolatorCurve(new_points,
                                             InterpolationMethodCurve::BEZIER);
  int                npts = edge_divisions * (int)this->get_npoints();
  std::vector<float> t = hmap::linspace(0.f, 1.f, npts);

  Path new_path = Path(fitp(t));
  *this = std::move(new_path);
}

void Path::bezier_round(float curvature_ratio, int edge_divisions)
{
  // --- generate a new set of points by adding control points
  // --- inbetween path points

  std::vector<Point> new_points = {};
  new_points.reserve(this->get_npoints() + 2 * (this->get_npoints() - 1));

  size_t npoints = this->get_npoints();
  size_t end = this->closed ? npoints : npoints - 1;

  for (size_t k = 0; k < end; k++)
  {
    size_t kprev = (k - 1) % npoints;
    size_t knext = (k + 1) % npoints;
    size_t knext_after = (k + 2) % npoints;

    Point delta_p1 = this->points[knext] - this->points[kprev];
    Point delta_p2 = this->points[k] - this->points[knext_after];

    Point pc1 = this->points[k] + curvature_ratio * delta_p1;
    Point pc2 = this->points[knext] + curvature_ratio * delta_p2;

    new_points.push_back(this->points[k]);
    new_points.push_back(pc1);
    new_points.push_back(pc2);
  }

  // add **first** point to close the loop
  if (this->closed) new_points.push_back(this->points.front());

  // --- interpolate

  InterpolatorCurve  fitp = InterpolatorCurve(new_points,
                                             InterpolationMethodCurve::BEZIER);
  int                npts = edge_divisions * (int)this->get_npoints();
  std::vector<float> t = hmap::linspace(0.f, 1.f, npts);

  Path new_path = Path(fitp(t));
  *this = std::move(new_path);
}

void Path::bspline(int edge_divisions)
{
  InterpolatorCurve  fitp = InterpolatorCurve(this->points,
                                             InterpolationMethodCurve::BSPLINE);
  int                npts = edge_divisions * (int)this->get_npoints();
  std::vector<float> t = hmap::linspace(0.f, 1.f, npts);

  Path new_path = Path(fitp(t));

  *this = std::move(new_path);
}

void Path::catmullrom(int edge_divisions)
{
  InterpolatorCurve fitp = InterpolatorCurve(
      this->points,
      InterpolationMethodCurve::CATMULLROM);
  int                npts = edge_divisions * (int)this->get_npoints();
  std::vector<float> t = hmap::linspace(0.f, 1.f, npts);

  Path new_path = Path(fitp(t));

  *this = std::move(new_path);
}

void Path::clear()
{
  this->points.clear();
  this->closed = false;
}

void Path::decasteljau(int edge_divisions)
{
  std::vector<Point> new_points = this->points;

  if (this->closed) new_points.push_back(this->points.front());

  InterpolatorCurve fitp = InterpolatorCurve(
      new_points,
      InterpolationMethodCurve::DECASTELJAU);
  int                npts = edge_divisions * (int)new_points.size();
  std::vector<float> t = hmap::linspace(0.f, 1.f, npts);

  Path new_path = Path(fitp(t));

  *this = std::move(new_path);
}

void Path::decimate_cfit(int n_points_target)
{
  // compute local curvature
  std::vector<float> kappas = {};

  for (size_t k = 1; k < this->get_npoints() - 1; k++)
  {
    float kp = curvature(this->points[k - 1],
                         this->points[k],
                         this->points[k + 1]);
    kappas.push_back(kp);
  }

  // sort by size (ascending)
  std::vector<size_t> ksort = argsort(kappas);

  // rebuild simplified path
  std::vector<Point> new_points = {this->points.front()};
  size_t             klim = this->get_npoints() - (size_t)n_points_target;

  for (size_t k = 0; k < kappas.size(); k++)
  {
    if (ksort[k] >= klim) new_points.push_back(this->points[k + 1]);
  }
  new_points.push_back(this->points.back());

  *this = Path(new_points);
}

void Path::decimate_vw(int n_points_target)
{
  if (this->get_npoints() < 3 || this->get_npoints() <= (size_t)n_points_target)
    return;

  // compute triangle surfaces
  std::vector<float> surfaces = {};

  for (size_t k = 1; k < this->get_npoints() - 1; k++)
  {
    float s = triangle_area(this->points[k - 1],
                            this->points[k],
                            this->points[k + 1]);
    surfaces.push_back(s);
  }

  // sort by size (ascending)
  std::vector<size_t> ksort = argsort(surfaces);

  // rebuild simplified path
  std::vector<Point> new_points = {this->points.front()};
  size_t             klim = this->get_npoints() - (size_t)n_points_target;

  for (size_t k = 0; k < surfaces.size(); k++)
  {
    if (ksort[k] >= klim) new_points.push_back(this->points[k + 1]);
  }
  new_points.push_back(this->points.back());

  *this = Path(new_points);
}

void Path::dijkstra(Array      &array,
                    Vec4<float> bbox,
                    float       elevation_ratio,
                    float       distance_exponent,
                    float       upward_penalization,
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

    std::vector<int> ip, jp;
    find_path_dijkstra(array,
                       ij_start,
                       ij_end,
                       ip,
                       jp,
                       elevation_ratio,
                       distance_exponent,
                       upward_penalization,
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
  size_t npoints = this->get_npoints();
  size_t end = this->closed ? npoints : npoints - 1;

  std::vector<Point> new_points;
  new_points.reserve(2 * npoints); // reserve space for the new points

  for (size_t k = 0; k < end; ++k)
  {
    size_t knext = (k + 1) % npoints;

    // add the original point
    new_points.push_back(this->points[k]);

    // calculate and add the midpoint
    Point midpoint = lerp(this->points[k], this->points[knext], 0.5f);
    new_points.push_back(midpoint);
  }

  if (!this->closed) new_points.push_back(this->points.back());

  this->points = std::move(new_points);
}

void Path::fractalize(int         iterations,
                      uint        seed,
                      float       sigma,
                      int         orientation,
                      float       persistence,
                      Array      *p_ctrl_array,
                      Vec4<float> bbox)
{
  std::mt19937                    gen(seed);
  std::normal_distribution<float> dis(0.f, 1.f);

  for (int it = 0; it < iterations; it++)
  {
    std::vector<Point> new_points = {};

    // determine the ending index based on whether the list is
    // closed (circular)
    size_t npoints = this->get_npoints();
    size_t end = this->closed ? npoints : npoints - 1;

    for (size_t k = 0; k < end; k++)
    {
      // determine the index of the next point, wrapping around if circular
      size_t knext = (k + 1) % npoints;

      // generate random displacement amplitude (as a ratio to the
      // point distance)
      float amp = sigma * dis(gen);

      // if provided, modulate amplitude based on underlying field
      if (p_ctrl_array)
        amp *= p_ctrl_array->get_value_nearest(this->points[k].x,
                                               this->points[k].y,
                                               bbox);

      // insert midpoint between current edge start and end
      Point pnew = midpoint(this->points[k],
                            this->points[knext],
                            orientation,
                            amp);

      new_points.push_back(this->points[k]);
      new_points.push_back(pnew);
    }

    // if the path is not closed, ensure the last original point is added
    if (!this->closed) new_points.push_back(this->points.back());

    // replace the original points with the resampled points
    this->points = std::move(new_points);

    // update sigma by multiplying with persistence factor
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

std::vector<float> Path::get_values() const
{
  std::vector<float> v(this->get_npoints());
  for (size_t i = 0; i < this->get_npoints(); i++)
    v[i] = this->points[i].v;

  if (this->closed && this->get_npoints() > 0) v.push_back(this->points[0].v);
  return v;
}

std::vector<float> Path::get_x() const
{
  std::vector<float> x(this->get_npoints());
  for (size_t i = 0; i < this->get_npoints(); i++)
    x[i] = this->points[i].x;

  if (this->closed && this->get_npoints() > 0) x.push_back(this->points[0].x);
  return x;
}

std::vector<float> Path::get_xy() const
{
  std::vector<float> xy(2 * this->get_npoints());
  for (size_t i = 0; i < this->get_npoints(); i++)
  {
    xy[2 * i] = this->points[i].x;
    xy[2 * i + 1] = this->points[i].y;
  }

  if (this->closed && this->get_npoints() > 0)
  {
    xy.push_back(this->points[0].x);
    xy.push_back(this->points[0].y);
  }
  return xy;
}

std::vector<float> Path::get_y() const
{
  std::vector<float> y(this->get_npoints());
  for (size_t i = 0; i < this->get_npoints(); i++)
    y[i] = this->points[i].y;

  if (this->closed && this->get_npoints() > 0) y.push_back(this->points[0].y);
  return y;
}

void Path::enforce_monotonic_values(bool decreasing)
{
  if (decreasing)
  {
    for (size_t k = 0; k < this->get_npoints() - 1; k++)
    {
      if (this->points[k + 1].v > this->points[k].v)
        this->points[k + 1].v = this->points[k].v;
    }
  }
  else
  {
    for (size_t k = 0; k < this->get_npoints() - 1; k++)
    {
      if (this->points[k + 1].v < this->points[k].v)
        this->points[k + 1].v = this->points[k].v;
    }
  }
}

void Path::meanderize(float ratio,
                      float noise_ratio,
                      uint  seed,
                      int   iterations,
                      int   edge_divisions)
{
  std::mt19937                    gen(seed);
  std::normal_distribution<float> dis(-noise_ratio, noise_ratio);

  for (int it = 0; it < iterations; it++)
  {
    Path new_path = Path();

    float cross_product;

    if (this->get_npoints() > 1)
      cross_product = (this->points[2].y - this->points[0].y) *
                          (this->points[1].x - this->points[0].x) -
                      (this->points[2].x - this->points[0].x) *
                          (this->points[1].y - this->points[0].y);
    else
      cross_product = 1.f;

    float orientation = -std::copysign(1.f, cross_product);

    size_t ks = this->closed ? 0 : 1;
    for (size_t k = 0; k < this->get_npoints() - ks; k++)
    {
      size_t kp1 = (k + 1) % this->get_npoints();

      new_path.add_point(
          Point(this->points[k].x, this->points[k].y, this->points[k].v));

      float alpha = angle(this->points[kp1], this->points[k]);
      float dist = distance(this->points[kp1], this->points[k]);

      Point p = lerp(this->points[k], this->points[kp1], 0.5f);

      if (orientation >= 0.f)
        alpha += M_PI_2;
      else
        alpha -= M_PI_2;

      dist *= ratio * (1.f + dis(gen));

      p.x += dist * std::cos(alpha);
      p.y += dist * std::sin(alpha);

      new_path.add_point(p);
      orientation *= -1.f;
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
  // reserve space in idx vector upfront
  std::vector<int> idx;
  idx.reserve(this->get_npoints());
  idx.push_back(start_index);

  // populate the search queue with all other indices
  std::list<int> queue_search;
  for (int k = 0; k < static_cast<int>(this->get_npoints()); ++k)
    if (k != start_index) queue_search.push_back(k);

  while (idx.size() < this->get_npoints())
  {
    int   k = idx.back(); // current point
    int   knext = -1;     // next point to add to idx
    float dmin = std::numeric_limits<float>::max();

    for (const auto &i : queue_search)
    {
      float dist = distance(this->points[k], this->points[i]);
      if (dist < dmin)
      {
        dmin = dist;
        knext = i;
      }
    }

    // ensure knext was found
    if (knext != -1)
    {
      queue_search.remove(knext);
      idx.push_back(knext);
    }
  }

  // reorder the points based on the new indices
  std::vector<Point> reordered_points;
  reordered_points.reserve(this->get_npoints());
  for (const int &i : idx)
    reordered_points.push_back(this->points[i]);

  // assign the reordered points back to the object's points vector
  this->points = std::move(reordered_points);
}

void Path::resample(float delta)
{
  // determine the ending index based on whether the list is closed
  // (circular)
  size_t npoints = this->get_npoints();
  size_t end = this->closed ? npoints : npoints - 1;

  // initialize a vector to store the new resampled points
  std::vector<Point> new_points = {};

  // loop through each segment of the path
  for (size_t k = 0; k < end; k++)
  {
    // get the index of the next point, wrapping around if the path is closed
    size_t knext = (k + 1) % npoints;

    // calculate the number of divisions needed based on the distance
    // and the desired delta
    float dist = distance(this->points[k], this->points[knext]);

    int   ndiv = static_cast<int>(dist / delta);
    Point p1 = this->points[k];     // Current point
    Point p2 = this->points[knext]; // Next point

    // if more than one division is required, generate intermediate points
    if (ndiv > 1)
    {
      // generate linearly spaced interpolation factors between 0 and 1,
      // excluding the endpoint
      std::vector<float> t = linspace(0.f, 1.f, ndiv, false);

      for (auto &t_ : t)
        new_points.push_back(lerp(p1, p2, t_));
    }
    else
    {
      // if the segment is shorter than delta, just add the current point
      new_points.push_back(p1);
    }
  }

  // if the path is not closed, ensure the last original point is added
  if (!this->closed) new_points.push_back(this->points.back());

  // replace the original points with the resampled points
  this->points = std::move(new_points);
}

void Path::resample_uniform()
{
  // determine smallest distance between two consecutive points (and
  // store distances because there are used for the interpolation
  // step)
  float dmin = std::numeric_limits<float>::max();

  size_t npoints = this->get_npoints();
  size_t end = this->closed ? npoints : npoints - 1;

  for (size_t k = 0; k < end; k++)
  {
    size_t knext = (k + 1) % npoints;
    float  dist = distance(this->points[k], this->points[knext]);
    if (dist < dmin) dmin = dist;
  }

  // resample
  this->resample(dmin);
}

void Path::reverse()
{
  std::reverse(this->points.begin(), this->points.end());
}

float Path::sdf_angle_closed(float x, float y)
{
  float  d = std::numeric_limits<float>::max();
  size_t k_closest = 0;
  size_t kp_closest = 0;

  for (size_t i = 0, j = this->get_npoints() - 1; i < this->get_npoints();
       j = i, i++)
  {
    Vec2<float> e = {this->points[j].x - this->points[i].x,
                     this->points[j].y - this->points[i].y};
    Vec2<float> w = {x - this->points[i].x, y - this->points[i].y};
    float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
    Vec2<float> b = {w.x - e.x * coeff, w.y - e.y * coeff};
    float       di = dot(b, b);
    if (di < d)
    {
      d = di;
      k_closest = i;
      kp_closest = j;
    }
  }

  return k_closest == 0
             ? angle(this->points[k_closest], this->points[kp_closest])
             : angle(this->points[kp_closest], this->points[k_closest]);
}

float Path::sdf_angle_open(float x, float y)
{
  float  d = std::numeric_limits<float>::max();
  size_t k_closest = 0;

  for (size_t i = 0; i < this->get_npoints() - 1; i++)
  {
    size_t      j = i + 1;
    Vec2<float> e = {this->points[j].x - this->points[i].x,
                     this->points[j].y - this->points[i].y};
    Vec2<float> w = {x - this->points[i].x, y - this->points[i].y};
    float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
    Vec2<float> b = {w.x - e.x * coeff, w.y - e.y * coeff};
    float       di = dot(b, b);
    if (di <= d)
    {
      d = di;
      k_closest = i;
    }
  }
  return angle(this->points[k_closest], this->points[k_closest + 1]);
}

float Path::sdf_closed(float x, float y)
{
  float d = std::numeric_limits<float>::max();
  float s = 1.f;
  for (size_t i = 0, j = this->get_npoints() - 1; i < this->get_npoints();
       j = i, i++)
  {
    Vec2<float> e = {this->points[j].x - this->points[i].x,
                     this->points[j].y - this->points[i].y};
    Vec2<float> w = {x - this->points[i].x, y - this->points[i].y};
    float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
    Vec2<float> b = {w.x - e.x * coeff, w.y - e.y * coeff};
    d = std::min(d, dot(b, b));

    Vec3<bool> c = Vec3<bool>(y >= this->points[i].y,
                              y<this->points[j].y, e.x * w.y> e.y * w.x);

    if ((c.x && c.y && c.z) || (not(c.x) && not(c.y) && not(c.z))) s *= -1.f;
  }
  return s * std::sqrt(d);
}

float Path::sdf_elevation_closed(float x, float y, float slope)
{
  float d = -std::numeric_limits<float>::max();

  for (size_t i = 0, j = this->get_npoints() - 1; i < this->get_npoints();
       j = i, i++)
  {
    Vec2<float> e = {this->points[j].x - this->points[i].x,
                     this->points[j].y - this->points[i].y};
    Vec2<float> w = {x - this->points[i].x, y - this->points[i].y};
    float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
    Vec2<float> b = {w.x - e.x * coeff, w.y - e.y * coeff};
    float dtmp = (1.f - coeff) * this->points[i].v + coeff * this->points[j].v -
                 slope * std::sqrt(dot(b, b));
    d = std::max(d, dtmp);
  }
  return d;
}

float Path::sdf_elevation_open(float x, float y, float slope)
{
  float d = -std::numeric_limits<float>::max();

  for (size_t i = 0; i < this->get_npoints() - 1; i++)
  {
    size_t      j = i + 1;
    Vec2<float> e = {this->points[j].x - this->points[i].x,
                     this->points[j].y - this->points[i].y};
    Vec2<float> w = {x - this->points[i].x, y - this->points[i].y};
    float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
    Vec2<float> b = {w.x - e.x * coeff, w.y - e.y * coeff};
    float dtmp = (1.f - coeff) * this->points[i].v + coeff * this->points[j].v -
                 slope * std::sqrt(dot(b, b));
    d = std::max(d, dtmp);
  }
  return d;
}

float Path::sdf_open(float x, float y)
{
  // distance
  float d = std::numeric_limits<float>::max();

  for (size_t i = 0; i < this->get_npoints() - 1; i++)
  {
    size_t      j = i + 1;
    Vec2<float> e = {this->points[j].x - this->points[i].x,
                     this->points[j].y - this->points[i].y};
    Vec2<float> w = {x - this->points[i].x, y - this->points[i].y};
    float       coeff = std::clamp(dot(w, e) / dot(e, e), 0.f, 1.f);
    Vec2<float> b = {w.x - e.x * coeff, w.y - e.y * coeff};
    d = std::min(d, dot(b, b));
  }
  return std::sqrt(d);
}

void Path::smooth(int navg, float averaging_intensity, float inertia)
{
  // moving average (start and end points are not modified)
  std::vector<Point> smooth_points = {};
  smooth_points.reserve(this->get_npoints());

  for (int i = 0; i < (int)this->get_npoints(); i++)
  {
    int i1 = (i >= navg) ? navg : i;
    int i2 = (i <= (int)this->get_npoints() - 1 - navg)
                 ? navg
                 : (int)this->get_npoints() - 1 - i;
    int is = std::min(i1, i2);

    Point psum(0.f, 0.f);

    for (int k = i - is; k < i + is + 1; k++)
      psum = psum + this->points[k];

    Point new_point = psum / (float)(2 * is + 1);
    new_point = (1.f - averaging_intensity) * this->points[i] +
                averaging_intensity * new_point;

    smooth_points.push_back(new_point);
  }

  this->points = smooth_points;

  // inertia (same, start and end points are not modified)
  if (inertia > 0.f)
    for (size_t i = 1; i < this->get_npoints() - 1; i++)
      this->points[i] = (1.f - inertia) * this->points[i] +
                        inertia * this->points[i - 1];
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

void Path::to_array(Array &array, Vec4<float> bbox, bool filled) const
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
  if (filled) cloud.set_values(1.f);

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

Array Path::to_array_sdf(Vec2<int>   shape,
                         Vec4<float> bbox,
                         Array      *p_noise_x,
                         Array      *p_noise_y,
                         Vec4<float> bbox_array)
{
  // Path nodes
  std::vector<float> xp = this->get_x();
  std::vector<float> yp = this->get_y();

  for (size_t k = 0; k < xp.size(); k++)
  {
    xp[k] = (xp[k] - bbox.a) / (bbox.b - bbox.a);
    yp[k] = (yp[k] - bbox.c) / (bbox.d - bbox.c);
  }

  // fill heightmap
  std::function<float(float, float, float)> distance_fct;

  if (this->closed)
    distance_fct = [this](float x, float y, float)
    { return this->sdf_closed(x, y); };
  else
    distance_fct = [this](float x, float y, float)
    { return this->sdf_open(x, y); };

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

void Path::to_png(std::string fname, Vec2<int> shape)
{
  Array array = Array(shape);
  this->to_array(array, this->get_bbox());
  array.to_png(fname, Cmap::INFERNO, false);
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
  mask = distance_transform_approx(mask);
  mask = exp(-mask * mask * 0.5f / ((float)(decay * decay)));
  smooth_cpulse(mask, decay);

  zf += depth;
  z = lerp(z, zf, mask);
}

void dig_river(Array                   &z,
               const std::vector<Path> &path_list,
               float                    riverbank_talus,
               int                      river_width,
               int                      merging_width,
               float                    depth,
               float                    riverbed_talus,
               float                    noise_ratio,
               uint                     seed,
               Array                   *p_mask)
{
  // generate mask where the river path lies and dig rivers
  Array       mask(z.shape);
  hmap::Array z_carved(z.shape);

  for (auto path : path_list)
  {
    Path path_copy = path;
    path_copy.set_values(1.f);
    hmap::Vec4<float> bbox(0.f, 1.f, 0.f, 1.f);
    path_copy.to_array(mask, bbox);

    // expand the path
    path_copy = path;
    path_copy.enforce_monotonic_values();

    for (auto &p : path_copy.points)
      p.v -= depth;

    // add downstream slope
    if (riverbed_talus > 0.f)
    {
      for (size_t k = 0; k < path_copy.get_npoints() - 1; k++)
        path_copy.points[k + 1].v = std::min(path_copy.points[k + 1].v,
                                             path_copy.points[k].v -
                                                 riverbed_talus);
    }

    path_copy.to_array(z_carved, bbox);
  }

  if (river_width)
  {
    z_carved = dilation(z_carved, river_width);
    mask = dilation(mask, river_width);
  }

  for (int j = 0; j < z.shape.y; ++j)
    for (int i = 0; i < z.shape.x; ++i)
      z_carved(i, j) = mask(i, j) ? z_carved(i, j) : z(i, j);

  expand_talus(z_carved, mask, riverbank_talus, seed, noise_ratio);
  laplace(z_carved);

  // use a distance transform to define a merging mask between the
  // input heightmap "z" and the "z_carved"
  Array dist = distance_transform_approx(mask);
  float sigma2 = std::pow((float)(merging_width + river_width), 2.f);
  dist = exp(-0.5f * dist * dist / sigma2);
  laplace(dist);

  // lerp based on distance
  z = lerp(z, z_carved, dist);

  // return mask if requested
  if (p_mask) *p_mask = dist;
}

void dig_river(Array      &z,
               const Path &path,
               float       riverbank_talus,
               int         river_width,
               int         merging_width,
               float       depth,
               float       riverbed_talus,
               float       noise_ratio,
               uint        seed,
               Array      *p_mask)
{
  const std::vector<Path> path_list = {path};

  dig_river(z,
            path_list,
            riverbank_talus,
            river_width,
            merging_width,
            depth,
            riverbed_talus,
            noise_ratio,
            seed,
            p_mask);
}

} // namespace hmap
