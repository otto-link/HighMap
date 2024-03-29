/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/noise_function.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array biquad_pulse(Vec2<int>   shape,
                   float       gain,
                   Array      *p_noise_x,
                   Array      *p_noise_y,
                   Array      *p_stretching,
                   Vec4<float> bbox)
{
  Array array = Array(shape);
  float gain_inv = 1.f / gain;

  auto lambda = [&gain_inv](float x, float y, float)
  {
    float v = x * (x - 1.f) * y * (y - 1.f);
    v = std::clamp(v, 0.f, 1.f);
    return std::pow(v, gain_inv);
  };

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               lambda);
  return array;
}

Array bump(Vec2<int>   shape,
           float       gain,
           Array      *p_noise_x,
           Array      *p_noise_y,
           Array      *p_stretching,
           Vec4<float> bbox)
{
  Array array = Array(shape);
  float gain_inv = 1.f / gain;

  auto lambda = [&gain_inv](float x, float y, float)
  {
    float r2 = (x - 0.5f) * (x - 0.5f) + (y - 0.5f) * (y - 0.5f);
    return r2 > 0.25f ? 0.f
                      : std::pow(std::exp(-1.f / (1.f - 4.f * r2)), gain_inv);
  };

  fill_array_using_xy_function(array,
                               bbox,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               lambda);
  return array;
}

Array constant(Vec2<int> shape, float value)
{
  Array array = Array(shape);
  for (auto &v : array.vector)
    v = value;
  return array;
}

Array gaussian_pulse(Vec2<int>   shape,
                     float       sigma,
                     Array      *p_noise,
                     Vec2<float> center,
                     Vec4<float> bbox)
{
  Array array = Array(shape);

  Vec2<float> shift = {bbox.a, bbox.c};
  Vec2<float> scale = {bbox.b - bbox.a, bbox.d - bbox.c};

  int ic = (int)((center.x - shift.x) / scale.x * shape.x);
  int jc = (int)((center.y - shift.y) / scale.y * shape.y);

  float s2 = 1.f / (sigma * sigma);

  if (!p_noise)
  {
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
      {
        float r2 = (float)((i - ic) * (i - ic) + (j - jc) * (j - jc));
        array(i, j) = std::exp(-0.5f * r2 * s2);
      }
  }
  else
  {
    float noise_factor = (float)shape.x * (float)shape.y / scale.x / scale.y;

    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
      {
        float r2 = (float)((i - ic) * (i - ic) + (j - jc) * (j - jc)) +
                   (*p_noise)(i, j) * (*p_noise)(i, j) * noise_factor;
        array(i, j) = std::exp(-0.5f * r2 * s2);
      }
  }

  return array;
}

} // namespace hmap
