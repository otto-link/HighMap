/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <iostream>

#include "highmap/geometry/point.hpp"
#include "highmap/math.hpp"

namespace hmap
{

void Point::set_value_from_array(const Array &array, Vec4<float> bbox)
{
  // scale to unit interval
  float xn = (this->x - bbox.a) / (bbox.b - bbox.a);
  float yn = (this->y - bbox.c) / (bbox.d - bbox.c);

  // scale to array shape
  xn *= (float)(array.shape.x - 1);
  yn *= (float)(array.shape.y - 1);

  int i = (int)xn;
  int j = (int)yn;

  if (i >= 0 && i < array.shape.x && j >= 0 && j < array.shape.y)
  {
    float uu = xn - (float)i;
    float vv = yn - (float)j;
    this->v = array.get_value_bilinear_at(i, j, uu, vv);
  }
  else
    this->v = 0.f; // if outside array bounding box
}

void Point::print()
{
  std::cout << "(" << this->x << ", " << this->y << ", " << this->v << ")"
            << std::endl;
}

float angle(const Point &p1, const Point &p2)
{
  return std::atan2(p2.y - p1.y, p2.x - p1.x);
}

float angle(const Point &p0, const Point &p1, const Point &p2)
{
  // Calculate vectors
  float dx1 = p1.x - p0.x;
  float dy1 = p1.y - p0.y;
  float dx2 = p2.x - p0.x;
  float dy2 = p2.y - p0.y;

  // Compute the angle using the atan2 function to get the signed angle
  float angle1 = std::atan2(dy1, dx1);
  float angle2 = std::atan2(dy2, dx2);

  // Calculate the angle difference
  float angle = angle2 - angle1;

  // Normalize the angle to be in the range [-π, π]
  if (angle > M_PI)
    angle -= 2 * M_PI;
  else if (angle < -M_PI)
    angle += 2 * M_PI;

  return angle;
}

float cross_product(const Point &p0, const Point &p1, const Point &p2)
{
  // calculate vectors v1 = p1 - p0 and v2 = p2 - p0
  float v1x = p1.x - p0.x;
  float v1y = p1.y - p0.y;
  float v2x = p2.x - p0.x;
  float v2y = p2.y - p0.y;

  // calculate the 2D cross product v1 x v2
  return v1x * v2y - v1y * v2x;
}

float curvature(const Point &p1, const Point &p2, const Point &p3)
{
  float d = distance(p1, p2) * distance(p1, p3) * distance(p1, p3);

  if (d > 0.f)
    return 2.f * triangle_area(p1, p2, p3) / d;
  else
    return 0.f;
}

float distance(const Point &p1, const Point &p2)
{
  float dx = p1.x - p2.x;
  float dy = p1.y - p2.y;
  return std::hypot(dx, dy);
}

Point interp_bezier(const Point &p_start,
                    const Point &p_ctrl_start,
                    const Point &p_ctrl_end,
                    const Point &p_end,
                    float        t)
{
  // https://github.com/chen0040/cpp-spline
  Point pi = p_start;
  pi = pi +
       t * t * t *
           ((-1.f) * p_start + 3.f * p_ctrl_start - 3.f * p_ctrl_end + p_end);
  pi = pi + t * t * (3.f * p_start - 6.f * p_ctrl_start + 3.f * p_ctrl_end);
  pi = pi + t * ((-3.f) * p_start + 3.f * p_ctrl_start);
  return pi;
}

Point interp_bspline(const Point &p0,
                     const Point &p1,
                     const Point &p2,
                     const Point &p3,
                     float        t)
{
  // https://github.com/chen0040/cpp-spline
  Point pi;
  pi = t * t * t * (-1.f * p0 + 3.f * p1 - 3.f * p2 + p3) / 6.f;
  pi = pi + t * t * (3.f * p0 - 6.f * p1 + 3.f * p2) / 6.f;
  pi = pi + t * (-3.f * p0 + 3.f * p2) / 6.f;
  pi = pi + (p0 + 4.f * p1 + p2) / 6.f;
  return pi;
}

Point interp_catmullrom(const Point &p0,
                        const Point &p1,
                        const Point &p2,
                        const Point &p3,
                        float        t)
{
  // https://github.com/chen0040/cpp-spline
  Point pi = p1;
  pi = pi + t * t * t * ((-1) * p0 + 3 * p1 - 3 * p2 + p3) / 2;
  pi = pi + t * t * (2 * p0 - 5 * p1 + 4 * p2 - p3) / 2;
  pi = pi + t * ((-1) * p0 + p2) / 2;
  return pi;
}

Point interp_decasteljau(const std::vector<Point> &points, float t)
{
  if (points.size() == 1) return points[0];

  std::vector<Point> new_points;
  for (size_t i = 0; i < points.size() - 1; ++i)
    new_points.push_back(points[i] * (1 - t) + points[i + 1] * t);

  return interp_decasteljau(new_points, t);
}

Vec4<float> intersect_bounding_boxes(const Vec4<float> &bbox1,
                                     const Vec4<float> &bbox2)
{
  // Calculate the boundaries of the intersection
  float min_x = std::max(bbox1.a, bbox2.a);
  float max_x = std::min(bbox1.b, bbox2.b);
  float min_y = std::max(bbox1.c, bbox2.c);
  float max_y = std::min(bbox1.d, bbox2.d);

  // Check if there is an overlap
  if (min_x <= max_x && min_y <= max_y)
  {
    return Vec4<float>{min_x, max_x, min_y, max_y};
  }

  // else return an "impossible" bounding box with xmin > xmax and ymin > ymax
  return Vec4<float>(1.f, -1.f, 1.f, -1.f);
}

bool is_point_within_bounding_box(Point p, Vec4<float> bbox)
{
  return p.x >= bbox.a && p.x <= bbox.b && p.y >= bbox.c && p.y <= bbox.d;
}

bool is_point_within_bounding_box(float x, float y, Vec4<float> bbox)
{
  return x >= bbox.a && x <= bbox.b && y >= bbox.c && y <= bbox.d;
}

Point lerp(const Point &p1, const Point &p2, float t)
{
  return p1 + t * (p2 - p1);
}

Point midpoint(const Point &p1,
               const Point &p2,
               int          orientation,
               float        distance_ratio,
               float        t)
{
  // interpolated midpoint based on t
  Point pmid = lerp(p1, p2, t);

  // apply orientation (0 for random direction, 1 for inflation, -1
  // for deflation)
  distance_ratio = orientation == 0 ? distance_ratio
                                    : std::abs(distance_ratio) *
                                          std::copysign(1.0f, orientation);

  // normalize the perpendicular vector and apply the orientation and
  // distance ratio (NB - point distance is embedded in dx and dy)
  float dx = p2.x - p1.x;
  float dy = p2.y - p1.y;

  float perp_x = -dy * distance_ratio;
  float perp_y = dx * distance_ratio;

  // apply the perpendicular displacement to the midpoint
  Point displaced_midpoint(pmid.x + perp_x, pmid.y + perp_y, pmid.v);

  return displaced_midpoint;
}

Vec4<float> unit_square_bbox()
{
  return Vec4<float>(0.f, 1.f, 0.f, 1.f);
}

// HELPER
bool cmp_inf(Point &a, Point &b)
{
  return (a.x < b.x) | (a.x == b.x && a.y < b.y) |
         (a.x == b.x && a.y == b.y && a.v < b.v);
}

void sort_points(std::vector<Point> &points)
{
  std::sort(points.begin(), points.end(), cmp_inf);
}

float triangle_area(const Point &p1, const Point &p2, const Point &p3)
{
  return 0.5f * std::abs(p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) +
                         p3.x * (p1.y - p2.y));
}

} // namespace hmap
