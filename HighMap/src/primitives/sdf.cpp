/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

// taken from https://iquilezles.org/articles/distfunctions2d/

namespace hmap
{

Array sdf_circle(Vec2<int>   shape,
                 float       radius,
                 Array      *p_noise_x,
                 Array      *p_noise_y,
                 Vec2<float> center,
                 Vec2<float> shift,
                 Vec2<float> scale)
{
  auto distance_fct = [&radius](float x, float y)
  { return std::hypot(x, y) - radius; };

  std::vector<float> x = linspace(shift.x - center.x,
                                  scale.x - center.x + shift.x,
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(shift.y - center.y,
                                  scale.y - center.y + shift.y,
                                  shape.y,
                                  false);

  Array array = Array(shape);
  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               nullptr,
                               distance_fct);
  return array;
}

Array sdf_polyline(Vec2<int>          shape,
                   std::vector<float> xp,
                   std::vector<float> yp,
                   Array             *p_noise_x,
                   Array             *p_noise_y,
                   Vec2<float>        shift,
                   Vec2<float>        scale)
{
  Path path = Path(xp, yp);

  auto distance_fct = [&path](float x, float y) { return path.sdf_open(x, y); };

  std::vector<float> x = linspace(shift.x, scale.x + shift.x, shape.x, false);
  std::vector<float> y = linspace(shift.y, scale.y + shift.y, shape.y, false);

  Array array = Array(shape);
  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               nullptr,
                               distance_fct);
  return array;
}

Array sdf_polygon(Vec2<int>          shape,
                  std::vector<float> xp,
                  std::vector<float> yp,
                  Array             *p_noise_x,
                  Array             *p_noise_y,
                  Vec2<float>        shift,
                  Vec2<float>        scale)
{
  Path path = Path(xp, yp);

  auto distance_fct = [&path](float x, float y)
  { return path.sdf_closed(x, y); };

  std::vector<float> x = linspace(shift.x, scale.x + shift.x, shape.x, false);
  std::vector<float> y = linspace(shift.y, scale.y + shift.y, shape.y, false);

  Array array = Array(shape);
  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               nullptr,
                               distance_fct);
  return array;
}

Array sdf_polygon_annular(Vec2<int>          shape,
                          std::vector<float> xp,
                          std::vector<float> yp,
                          float              width,
                          Array             *p_noise_x,
                          Array             *p_noise_y,
                          Vec2<float>        shift,
                          Vec2<float>        scale)
{
  Path path = Path(xp, yp);

  auto distance_fct = [&path, &width](float x, float y)
  { return std::abs(path.sdf_closed(x, y) - width); };

  std::vector<float> x = linspace(shift.x, scale.x + shift.x, shape.x, false);
  std::vector<float> y = linspace(shift.y, scale.y + shift.y, shape.y, false);

  Array array = Array(shape);
  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               nullptr,
                               distance_fct);
  return array;
}

} // namespace hmap
