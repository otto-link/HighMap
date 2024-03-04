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
                   Vec2<float> shift,
                   Vec2<float> scale)
{
  Array z = Array(shape);

  std::vector<float> x = linspace(shift.x - 0.5f,
                                  shift.x + scale.x - 0.5f,
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(shift.y - 0.5f,
                                  shift.y + scale.y - 0.5f,
                                  shape.y,
                                  false);

  float gain_inv = 1.f / gain;

  auto lambda = [&gain_inv](float x_, float y_, float)
  {
    float v = (x_ * x_ - 0.25f) * (y_ * y_ - 0.25f);
    v = std::clamp(v, 0.f, 1.f);
    return std::pow(v, gain_inv);
  };

  fill_array_using_xy_function(z, x, y, p_noise_x, p_noise_y, nullptr, lambda);

  return z;
}

Array bump(Vec2<int>   shape,
           float       gain,
           Array      *p_noise_x,
           Array      *p_noise_y,
           Vec2<float> shift,
           Vec2<float> scale)
{
  Array z = Array(shape);

  std::vector<float> x = linspace(shift.x - 0.5f,
                                  shift.x + scale.x - 0.5f,
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(shift.y - 0.5f,
                                  shift.y + scale.y - 0.5f,
                                  shape.y,
                                  false);

  float gain_inv = 1.f / gain;

  auto lambda = [&gain_inv](float x_, float y_, float)
  {
    float r2 = x_ * x_ + y_ * y_;
    return r2 > 0.25f ? 0.f
                      : std::pow(std::exp(-1.f / (1.f - 4.f * r2)), gain_inv);
  };

  fill_array_using_xy_function(z, x, y, p_noise_x, p_noise_y, nullptr, lambda);

  return z;
}

Array bump_field(Vec2<int> shape, float kw, uint seed, float shape_factor)
{
  Array z = Array(shape);
  float density = kw * kw / (float)(shape.x * shape.y);
  float rmax = shape_factor * (float)std::min(shape.x, shape.y) / kw;

  z = white_sparse(shape, 0.f, 1.f, density, seed);
  z = distance_transform(z);

  clamp_max(z, rmax);
  z /= rmax;
  z = 1.f - z * z * (2.f - z);

  return z;
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
                     Vec2<float> shift,
                     Vec2<float> scale)
{
  Array array = Array(shape);

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
