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

// HELPER
Array sdf_generic(Vec2<int>                          shape,
                  std::function<float(float, float)> distance_fct,
                  Array                             *p_noise_x,
                  Array                             *p_noise_y,
                  Vec2<float>                        center,
                  Vec2<float>                        shift,
                  Vec2<float>                        scale)
{
  Array array = Array(shape);

  std::vector<float> x = linspace(shift.x - center.x,
                                  scale.x - center.x + shift.x,
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(shift.y - center.y,
                                  scale.y - center.y + shift.y,
                                  shape.y,
                                  false);

  if (p_noise_x && p_noise_y)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = distance_fct(x[i] + (*p_noise_x)(i, j),
                                   y[j] + (*p_noise_y)(i, j));
  else if (p_noise_x)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = distance_fct(x[i] + (*p_noise_x)(i, j), y[j]);
  else if (p_noise_y)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = distance_fct(x[i], y[j] + (*p_noise_y)(i, j));
  else
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = distance_fct(x[i], y[j]);

  return array;
}

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

  return sdf_generic(shape,
                     distance_fct,
                     p_noise_x,
                     p_noise_y,
                     center,
                     shift,
                     scale);
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

  return sdf_generic(shape,
                     distance_fct,
                     p_noise_x,
                     p_noise_y,
                     Vec2<float>(0.f, 0.f), // center at bottom-left
                     shift,
                     scale);
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

  return sdf_generic(shape,
                     distance_fct,
                     p_noise_x,
                     p_noise_y,
                     Vec2<float>(0.f, 0.f), // center at bottom-left
                     shift,
                     scale);
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

  return sdf_generic(shape,
                     distance_fct,
                     p_noise_x,
                     p_noise_y,
                     Vec2<float>(0.f, 0.f), // center at bottom-left
                     shift,
                     scale);
}

} // namespace hmap
