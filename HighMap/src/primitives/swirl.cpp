/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

void swirl(Array            &dx,
           Array            &dy,
           float             amplitude,
           float             exponent,
           const Array      *p_noise,
           hmap::Vec4<float> bbox)
{
  const Vec2<int> shape = dx.shape;

  Vec2<float> shift = {bbox.a, bbox.c};
  Vec2<float> scale = {bbox.b - bbox.a, bbox.d - bbox.c};

  float xc = (0.5f - shift.x) / scale.x;
  float yc = (0.5f - shift.y) / scale.y;

  if (!p_noise)
  {
    for (int j = 0; j < shape.y; j++)
      for (int i = 0; i < shape.x; i++)
      {
        float x = (float)i / (float)(shape.x - 1) * scale.x - xc;
        float y = (float)j / (float)(shape.y - 1) * scale.y - yc;
        float re = std::pow(2.f * (x * x + y * y), 0.5f * exponent);
        float theta = 2.f * M_PI * amplitude * re;

        dx(i, j) = std::cos(theta);
        dy(i, j) = std::sin(theta);
      }
  }
  else
  {
    for (int j = 0; j < shape.y; j++)
      for (int i = 0; i < shape.x; i++)
      {
        float x = (float)i / (float)(shape.x - 1) * scale.x - xc;
        float y = (float)j / (float)(shape.y - 1) * scale.y - yc;
        float re = std::pow(2.f * (x * x + y * y), 0.5f * exponent);
        float theta = 2.f * M_PI * (amplitude * re + (*p_noise)(i, j));

        dx(i, j) = std::cos(theta);
        dy(i, j) = std::sin(theta);
      }
  }
}

} // namespace hmap
