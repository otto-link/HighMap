/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/geometry/point.hpp"

namespace hmap
{

void Point::update_value_from_array(const Array &array, Vec4<float> bbox)
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

float distance(const Point &p1, const Point &p2)
{
  float dx = p1.x - p2.x;
  float dy = p1.y - p2.y;
  return std::hypot(dx, dy);
}

Point lerp(const Point &p1, const Point &p2, float t)
{
  // Clamp t to the range [0, 1] to restrict interpolation between p1 and p2
  t = std::clamp(t, 0.0f, 1.0f);
  return p1 + t * (p2 - p1);
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

} // namespace hmap
