#include <cmath>

#include "highmap/geometry.hpp"

namespace hmap
{

float angle(const Point &p1, const Point &p2)
{
  return std::atan2(p2.y - p1.y, p2.x - p1.x);
}

float distance(const Point &p1, const Point &p2)
{
  return std::hypot(p1.x - p2.x, p1.y - p2.y);
}

Point lerp(const Point &p1, const Point &p2, const float t)
{
  float x = (1.f - t) * p1.x + t * p2.x;
  float y = (1.f - t) * p1.y + t * p2.y;
  float v = (1.f - t) * p1.v + t * p2.v;
  return Point(x, y, v);
}

} // namespace hmap
