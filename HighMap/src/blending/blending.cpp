/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/math.hpp"
#include "highmap/range.hpp"

namespace hmap
{

Array blend_exclusion(const Array &array1, const Array &array2)
{
  Array array_out = Array(array1.shape);
  array_out = 0.5f - 2.f * (-0.5f + array1) * (-0.5f + array2);
  return array_out;
}

Array blend_gradients(const Array &array1, const Array &array2, int ir)
{
  Array dn1 = gradient_norm(array1);
  Array dn2 = gradient_norm(array2);

  smooth_cpulse(dn1, ir);
  smooth_cpulse(dn2, ir);

  Array t = maximum_smooth(dn1, dn2, 0.1f);
  remap(t);

  return lerp(array1, array2, t);
}

Array blend_negate(const Array &array1, const Array &array2)
{
  Array array_out = Array(array1.shape);

  auto lambda = [](float a, float b) { return a < b ? a : 2.f * b - a; };

  std::transform(array1.vector.begin(),
                 array1.vector.end(),
                 array2.vector.begin(),
                 array_out.vector.begin(),
                 lambda);

  return array_out;
}

Array blend_overlay(const Array &array1, const Array &array2)
{
  Array array_out = Array(array1.shape);
  auto  lambda = [](float a, float b)
  { return a < 0.5 ? 2.f * a * b : 1.f - 2.f * (1.f - a) * (1.f - b); };

  std::transform(array1.vector.begin(),
                 array1.vector.end(),
                 array2.vector.begin(),
                 array_out.vector.begin(),
                 lambda);

  return array_out;
}

Array blend_soft(const Array &array1, const Array &array2)
{
  Array array_out = Array(array1.shape);
  array_out = (1.f - array1) * array1 * array2 +
              array1 * (1.f - (1.f - array1) * (1.f - array2));
  return array_out;
}

Array mixer(const Array                      &t,
            const std::vector<const Array *> &arrays,
            float                             gain_factor)
{
  Array      array_out = Array(t.shape);
  const uint n = arrays.size();

  for (uint k = 0; k < n; k++)
  {
    float r0 = (float)k / (float)(n - 1);

    if (gain_factor == 1.f)
    {
      for (int j = 0; j < t.shape.y; j++)
        for (int i = 0; i < t.shape.x; i++)
        {
          float ta = 1.f - std::fabs(t(i, j) - r0) * (float)(n - 1);
          if (ta >= 0.f)
          {
            float ts = ta * ta * (3.f - 2.f * ta);
            array_out(i, j) += ts * (*arrays[k])(i, j);
          }
        }
    }
    else
    {
      for (int j = 0; j < t.shape.y; j++)
        for (int i = 0; i < t.shape.x; i++)
        {
          float ta = 1.f - std::fabs(t(i, j) - r0) * (float)(n - 1);
          if (ta >= 0.f)
          {
            float ts = ta * ta * (3.f - 2.f * ta);
            ts = gain(ts, gain_factor);
            array_out(i, j) += ts * (*arrays[k])(i, j);
          }
        }
    }
  }
  return array_out;
}

} // namespace hmap
