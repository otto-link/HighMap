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

void directional_blur(Array &array, int ir, const Array &angle, float intensity)
{
  // create interpolation function
  ArrayFunction f = hmap::ArrayFunction(array, Vec2<float>(1.f, 1.f), true);

  std::vector<float> t = linspace(intensity, 0.f, ir);

  float tsum = 0.f;
  for (auto &v : t)
  {
    v = smoothstep3(v);
    tsum += v;
  }

  Array blured = array;

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      for (int k = 0; k < ir; k++)
      {
        float alpha = angle(i, j) / 180.f * M_PI;
        float ca = std::cos(alpha);
        float sa = std::sin(alpha);

        float x = (float)(i + k * ca) / (array.shape.x - 1.f);
        float y = (float)(j + k * sa) / (array.shape.y - 1.f);

        blured(i, j) += t[k] * f.get_delegate()(x, y, 0.f);
      }

  // try to rescale output
  array = blured / (1.f + tsum);
}

void directional_blur(Array &array, int ir, float angle, float intensity)
{
  directional_blur(array, ir, Array(array.shape, angle), intensity);
}

} // namespace hmap
