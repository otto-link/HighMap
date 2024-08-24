/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/interpolate_curve.hpp"

namespace hmap
{

// helper
int find_last_index_smaller_than(const std::vector<float> &vec, float threshold)
{
  size_t last_index = -1;

  for (size_t i = 0; i < vec.size(); i++)
    if (vec[i] <= threshold) last_index = i;

  // return the index, or -1 if no element was found.
  return (int)last_index;
}

InterpolatorCurve::InterpolatorCurve(std::vector<Point>       points,
                                     InterpolationMethodCurve method)
    : points_data(points), method(method)
{

  // normalized cumulaive distance of each points
  this->arc_length.reserve(this->points_data.size());
  this->arc_length.push_back(0.f);

  for (size_t k = 1; k < this->points_data.size(); k++)
  {
    float dist = distance(this->points_data[k], this->points_data[k - 1]);
    this->arc_length.push_back(this->arc_length.back() + dist);
  }

  // normalize
  for (auto &v : this->arc_length)
    v /= this->arc_length.back();

  // select the appropriate interpolation method
  switch (method)
  {

  case InterpolationMethodCurve::POINTS_LERP:
    this->interp = [this](float t)
    {
      if (t == 0.f)
        return this->points_data.front();
      else if (t == 1.f)
        return this->points_data.back();
      else
      {
        size_t i1;
        float  u;
        this->get_segment_interpolation_parameters(t, i1, u);

        return lerp(this->points_data[i1], this->points_data[i1 + 1], u);
      }
    };
    break;

  case InterpolationMethodCurve::BEZIER:
    this->interp = [this](float t)
    {
      if (t == 0.f)
        return this->points_data.front();
      else if (t == 1.f)
        return this->points_data.back();
      else
      {
        // find closest lowest multiple of 3 "to the left" of the
        // interpolation query (since Bezier is based on 4 continuous
        // points with the end point also used as a start point for
        // the next segment...)
        size_t i1 = find_last_index_smaller_than(this->arc_length, t);
        i1 = (i1 / 3) * 3;

        // convert to a local linear arc length
        float u = (t - this->arc_length[i1]) /
                  (this->arc_length[i1 + 3] - this->arc_length[i1]);

        if (i1 < this->points_data.size() - 3)
          return interp_bezier(this->points_data[i1],
                               this->points_data[i1 + 1],
                               this->points_data[i1 + 2],
                               this->points_data[i1 + 3],
                               u);
        else
          return this->points_data[i1];
      }
    };
    break;

  case InterpolationMethodCurve::BSPLINE:
    this->interp = [this](float t)
    {
      if (t == 0.f)
        return this->points_data.front();
      else if (t == 1.f)
        return this->points_data.back();
      else
      {
        size_t i1;
        float  u;
        this->get_segment_interpolation_parameters(t, i1, u);

        if (i1 > 0 && i1 < this->points_data.size() - 2)
          return interp_bspline(this->points_data[i1 - 1],
                                this->points_data[i1],
                                this->points_data[i1 + 1],
                                this->points_data[i1 + 2],
                                u);
        else if (i1 == 0)
          return interp_bspline(this->points_data[i1],
                                this->points_data[i1],
                                this->points_data[i1 + 1],
                                this->points_data[i1 + 2],
                                u);
        else if (i1 == this->points_data.size() - 2)
          return interp_bspline(this->points_data[i1 - 1],
                                this->points_data[i1],
                                this->points_data[i1 + 1],
                                this->points_data[i1 + 1],
                                u);
      }
    };
    break;

  case InterpolationMethodCurve::CATMULLROM:
    this->interp = [this](float t)
    {
      if (t == 0.f)
        return this->points_data.front();
      else if (t == 1.f)
        return this->points_data.back();
      else
      {
        size_t i1;
        float  u;
        this->get_segment_interpolation_parameters(t, i1, u);

        if (i1 > 0 && i1 < this->points_data.size() - 2)
          return interp_catmullrom(this->points_data[i1 - 1],
                                   this->points_data[i1],
                                   this->points_data[i1 + 1],
                                   this->points_data[i1 + 2],
                                   u);
        else if (i1 == 0)
          return interp_catmullrom(this->points_data[i1],
                                   this->points_data[i1],
                                   this->points_data[i1 + 1],
                                   this->points_data[i1 + 2],
                                   u);
        else if (i1 == this->points_data.size() - 2)
          return interp_catmullrom(this->points_data[i1 - 1],
                                   this->points_data[i1],
                                   this->points_data[i1 + 1],
                                   this->points_data[i1 + 1],
                                   u);
      }
    };
    break;

  case InterpolationMethodCurve::DECASTELJAU:
    this->interp = [this](float t)
    {
      if (t == 0.f)
        return this->points_data.front();
      else if (t == 1.f)
        return this->points_data.back();
      else
        return interp_decasteljau(this->points_data, t);
    };
    break;
  }
}

std::vector<Point> InterpolatorCurve::operator()(std::vector<float> t) const
{
  std::vector<Point> points = {};
  points.reserve(t.size());

  for (auto &t_ : t)
    points.push_back(this->interp(t_));

  return points;
}

void InterpolatorCurve::get_segment_interpolation_parameters(float   t,
                                                             size_t &ileft,
                                                             float  &u)
{
  // find closest points "to the left"
  ileft = find_last_index_smaller_than(this->arc_length, t);

  // interpolation parameter for this segment
  u = (t - this->arc_length[ileft]) /
      (this->arc_length[ileft + 1] - this->arc_length[ileft]);
}

} // namespace hmap
