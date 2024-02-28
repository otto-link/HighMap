/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
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

Array slope(Vec2<int>   shape,
            float       angle,
            float       talus,
            Array      *p_noise,
            Vec2<float> center,
            Vec2<float> shift,
            Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(shift.x - center.x,
                                  scale.x - center.x + shift.x,
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(shift.y - center.y,
                                  scale.y - center.y + shift.y,
                                  shape.y,
                                  false);

  // talus value for a unit square domain
  float talus_n = talus * std::max(shape.x / scale.x, shape.y / scale.y);
  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&talus_n](float x) { return talus_n * x; };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j));
  else
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j]);

  return array;
}

Array slope_x(Vec2<int>   shape,
              float       talus,
              Array      *p_noise,
              Vec2<float> shift,
              Vec2<float> scale)
{
  Array array = Array(shape);

  if (!p_noise)
  {
    for (int i = 0; i < array.shape.x; i++)
    {
      float h = talus * ((float)i + shift.x / scale.x * (float)shape.x);
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = h;
    }
  }
  else
  {
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float h = talus * ((float)i + (shift.x + (*p_noise)(i, j)) / scale.x *
                                          (float)shape.x);
        array(i, j) = h;
      }
  }
  return array;
}

Array slope_y(Vec2<int>   shape,
              float       talus,
              Array      *p_noise,
              Vec2<float> shift,
              Vec2<float> scale)
{
  Array array = Array(shape);

  if (!p_noise)
  {
    for (int j = 0; j < array.shape.y; j++)
    {
      float h = talus * ((float)j + shift.y / scale.y * (float)shape.y);
      for (int i = 0; i < array.shape.x; i++)
        array(i, j) = h;
    }
  }
  else
  {
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float h = talus * ((float)j + (shift.y + (*p_noise)(i, j)) / scale.y *
                                          (float)shape.y);
        array(i, j) = h;
      }
  }
  return array;
}

Array step(Vec2<int>   shape,
           float       angle,
           float       talus,
           Array      *p_noise,
           Vec2<float> center,
           Vec2<float> shift,
           Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(shift.x - center.x,
                                  scale.x - center.x + shift.x,
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(shift.y - center.y,
                                  scale.y - center.y + shift.y,
                                  shape.y,
                                  false);

  // talus value for a unit square domain
  float talus_n = talus * std::max(shape.x / scale.x, shape.y / scale.y);
  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);
  float dt = 0.5f / talus_n;

  auto lambda = [&dt, &talus_n](float x)
  {
    if (x > dt)
      x = 1.f;
    else if (x > -dt)
      x = talus_n * (x + dt);
    else
      x = 0.f;
    return x * x * (3.f - 2.f * x);
  };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j));
  else
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j]);

  return array;
}

Array wave_dune(Vec2<int>   shape,
                float       kw,
                float       angle,
                float       xtop,
                float       xbottom,
                float       phase_shift,
                Array      *p_noise,
                Vec2<float> shift,
                Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f + shift.x,
                                  scale.x + shift.x,
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(0.f + shift.y,
                                  scale.y + shift.y,
                                  shape.y,
                                  false);

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw, &phase_shift, &xtop, &xbottom](float x)
  {
    float xp = std::fmod(kw * x + phase_shift + 10.f, 1.f);
    float yp = 0.f;

    if (xp < xtop)
    {
      float r = xp / xtop;
      yp = r * r * (3.f - 2.f * r);
    }
    else if (xp < xbottom)
    {
      float r = (xp - xbottom) / (xtop - xbottom);
      yp = r * r * (2.f - r);
    }
    return yp;
  };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j));
  else
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j]);

  return array;
}

Array wave_sine(Vec2<int>   shape,
                float       kw,
                float       angle,
                float       phase_shift,
                Array      *p_noise,
                Vec2<float> shift,
                Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f + shift.x,
                                  scale.x + shift.x,
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(0.f + shift.y,
                                  scale.y + shift.y,
                                  shape.y,
                                  false);
  float              ca = std::cos(angle / 180.f * M_PI);
  float              sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw, &phase_shift](float x)
  { return std::cos(2.f * M_PI * kw * x + phase_shift); };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j) / kw);
  else
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j]);

  return array;
}

Array wave_square(Vec2<int>   shape,
                  float       kw,
                  float       angle,
                  float       phase_shift,
                  Array      *p_noise,
                  Vec2<float> shift,
                  Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f + shift.x,
                                  scale.x + shift.x,
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(0.f + shift.y,
                                  scale.y + shift.y,
                                  shape.y,
                                  false);
  float              ca = std::cos(angle / 180.f * M_PI);
  float              sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw](float x)
  { return x = 2.f * (int)(kw * x) - (int)(2.f * kw * x) + 1.f; };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j) / kw +
                             phase_shift);
  else
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + phase_shift);

  return array;
}

Array wave_triangular(Vec2<int>   shape,
                      float       kw,
                      float       angle,
                      float       slant_ratio,
                      float       phase_shift,
                      Array      *p_noise,
                      Vec2<float> shift,
                      Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f + shift.x,
                                  scale.x + shift.x,
                                  shape.x,
                                  false);
  std::vector<float> y = linspace(0.f + shift.y,
                                  scale.y + shift.y,
                                  shape.y,
                                  false);
  float              ca = std::cos(angle / 180.f * M_PI);
  float              sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw, &slant_ratio](float x)
  {
    x = kw * x - (int)(kw * x);
    if (x < slant_ratio)
      x /= slant_ratio;
    else
      x = 1.f - (x - slant_ratio) / (1.f - slant_ratio);
    return x * x * (3.f - 2.f * x);
  };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j) / kw +
                             phase_shift);
  else
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + phase_shift);

  return array;
}

} // namespace hmap
