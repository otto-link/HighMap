/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/functions.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

void directional_blur(Array       &array,
                      int          ir,
                      const Array &angle,
                      float        intensity,
                      float        stretch,
                      float        spread)
{
  // create interpolation function
  bool          periodic = false;
  ArrayFunction f = hmap::ArrayFunction(array, Vec2<float>(1.f, 1.f), periodic);

  std::vector<float> t = linspace(0.f, spread, ir);

  float tsum = 0.f;
  for (auto &v : t)
  {
    v = intensity * (1.f - smoothstep3(v));
    tsum += v;
  }

  Array blured = array;

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      for (int k = -ir + 1; k < ir; k++)
      {
        float alpha = angle(i, j) / 180.f * M_PI;
        float ca = std::cos(alpha);
        float sa = std::sin(alpha);
        float kr = stretch * (float)k;

        float x = (i + kr * ca) / (array.shape.x - 1.f);
        float y = (j + kr * sa) / (array.shape.y - 1.f);

        blured(i, j) += t[std::abs(k)] * f.get_delegate()(x, y, 0.f);
      }

  // try to rescale output
  array = blured / (1.f + 2.f * tsum);
}

void directional_blur(Array &array,
                      int    ir,
                      float  angle,
                      float  intensity,
                      float  stretch,
                      float  spread)
{
  directional_blur(array,
                   ir,
                   Array(array.shape, angle),
                   intensity,
                   stretch,
                   spread);
}

} // namespace hmap
