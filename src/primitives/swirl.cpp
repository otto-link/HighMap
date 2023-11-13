/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

void swirl(Array      &dx,
           Array      &dy,
           float       amplitude,
           float       exponent,
           Array      *p_noise,
           Vec2<float> shift,
           Vec2<float> scale)
{
  const Vec2<int> shape = dx.shape;

  float xc = (0.5f - shift.x) / scale.x;
  float yc = (0.5f - shift.y) / scale.y;

  if (!p_noise)
  {
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
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
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
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
