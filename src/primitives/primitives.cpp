/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array biweight(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float xi = ((float)i - ri) / ((float)(ri + 1));
      float yi = ((float)j - rj) / ((float)(rj + 1));
      float r2 = xi * xi + yi * yi;
      if (r2 < 1.f)
        array(i, j) = (1.f - r2) * (1.f - r2);
    }

  return array;
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

Array cone(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      array(i, j) = std::max(0.f, 1.f - r);
    }

  return array;
}

Array cone_smooth(Vec2<int> shape)
{
  Array array = cone(shape);
  almost_unit_identity(array);
  return array;
}

Array cone_talus(float height, float talus)
{
  // define output array size so that starting from an amplitude h,
  // zero is indeed reached with provided slope (talus) over the
  // half-width of the domain (since we build a cone)
  int   n = std::max(1, (int)(2.f * height / talus));
  Array array = Array({n, n});

  if (n > 0)
    array = height * cone({n, n});
  else
    array = 1.f;

  return array;
}

Array cubic_pulse(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = std::hypot(xi / float(ri + 1), yi / float(rj + 1));

      if (r < 1.f)
        array(i, j) = 1.f - r * r * (3.f - 2.f * r);
    }

  return array;
}

Array disk(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      if ((i - ri) * (i - ri) + (j - rj) * (j - rj) <= ri * rj)
        array(i, j) = 1.f;
    }

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
                     Vec2<float> shift,
                     Vec2<float> scale)
{
  Array array = Array(shape);

  int ic = (int)((0.5f - shift.x) / scale.x * shape.x);
  int jc = (int)((0.5f - shift.y) / scale.y * shape.y);

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

Array plane(Vec2<int>   shape,
            float       talus,
            float       yaw_angle,
            Vec3<float> xyz_center)
{
  Array array = Array(shape);
  // TODO !!!
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

Array smooth_cosine(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = M_PI * std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      if (r < M_PI)
        array(i, j) = 0.5f + 0.5f * std::cos(r);
    }

  return array;
}

Array tricube(Vec2<int> shape)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float xi = (float)i - ri;
      float yi = (float)j - rj;
      float r = std::hypot(xi / float(ri + 1), yi / float(rj + 1));
      if (r < 1.f)
        array(i, j) = std::pow(1.f - std::pow(r, 3.f), 3.f);
    }

  return array;
}

Array step(Vec2<int>   shape,
           float       angle,
           float       talus,
           Array      *p_noise,
           Vec2<float> shift,
           Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(-0.5f + shift.x,
                                  -0.5f + scale.x + shift.x,
                                  shape.x);
  std::vector<float> y = linspace(-0.5f + shift.y,
                                  -0.5f + scale.y + shift.y,
                                  shape.y);

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
    return x;
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
                Array      *p_noise,
                Vec2<float> shift,
                Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f + shift.x, scale.x + shift.x, shape.x);
  std::vector<float> y = linspace(0.f + shift.y, scale.y + shift.y, shape.y);
  float              ca = std::cos(angle / 180.f * M_PI);
  float              sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw](float x) { return std::cos(2.f * M_PI * kw * x); };

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

Array wave_square(Vec2<int>   shape,
                  float       kw,
                  float       angle,
                  Array      *p_noise,
                  Vec2<float> shift,
                  Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f + shift.x, scale.x + shift.x, shape.x);
  std::vector<float> y = linspace(0.f + shift.y, scale.y + shift.y, shape.y);
  float              ca = std::cos(angle / 180.f * M_PI);
  float              sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw](float x)
  { return 2.f * (int)(kw * x) - (int)(2.f * kw * x) + 1.f; };

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

Array wave_triangular(Vec2<int>   shape,
                      float       kw,
                      float       angle,
                      float       slant_ratio,
                      Array      *p_noise,
                      Vec2<float> shift,
                      Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x = linspace(0.f + shift.x, scale.x + shift.x, shape.x);
  std::vector<float> y = linspace(0.f + shift.y, scale.y + shift.y, shape.y);
  float              ca = std::cos(angle / 180.f * M_PI);
  float              sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw, &slant_ratio](float x)
  {
    x = kw * x - (int)(kw * x);
    if (x < slant_ratio)
      x /= slant_ratio;
    else
      x = 1.f - (x - slant_ratio) / (1.f - slant_ratio);
    return x;
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

//----------------------------------------------------------------------
// Helper
//----------------------------------------------------------------------

void helper_get_noise(Array                             &array,
                      std::vector<float>                &x,
                      std::vector<float>                &y,
                      Array                             *p_noise_x,
                      Array                             *p_noise_y,
                      std::function<float(float, float)> noise_fct)
{
  Vec2<int> shape = array.shape;

  if ((!p_noise_x) and (!p_noise_y))
  {
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
        array(i, j) = noise_fct(x[i], y[j]);
  }
  else if (p_noise_x and (!p_noise_y))
  {
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
        array(i, j) = noise_fct(x[i] + (*p_noise_x)(i, j), y[j]);
  }
  else if ((!p_noise_x) and p_noise_y)
  {
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
        array(i, j) = noise_fct(x[i], y[j] + (*p_noise_y)(i, j));
  }
  else if (p_noise_x and p_noise_y)
  {
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
        array(i, j) = noise_fct(x[i] + (*p_noise_x)(i, j),
                                y[j] + (*p_noise_y)(i, j));
  }
}

void helper_get_noise(Array                             &array,
                      Vec2<float>                        kw,
                      Array                             *p_noise_x,
                      Array                             *p_noise_y,
                      Vec2<float>                        shift,
                      Vec2<float>                        scale,
                      std::function<float(float, float)> noise_fct)
{
  float ki = kw.x / (float)array.shape.x * scale.x;
  float kj = kw.y / (float)array.shape.y * scale.y;

  if ((!p_noise_x) and (!p_noise_y)) // no noise
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float x = ki * (float)i + kw.x * shift.x;
        float y = kj * (float)j + kw.y * shift.y;
        array(i, j) = noise_fct(x, y);
      }
  else if (p_noise_x and (!p_noise_y)) // noise x only
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float x = ki * (float)i + kw.x * shift.x + (*p_noise_x)(i, j);
        float y = kj * (float)j + kw.y * shift.y;
        array(i, j) = noise_fct(x, y);
      }
  else if ((!p_noise_x) and p_noise_y) // noise y only
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float x = ki * (float)i + kw.x * shift.x;
        float y = kj * (float)j + kw.y * shift.y + (*p_noise_y)(i, j);
        array(i, j) = noise_fct(x, y);
      }
  else if (p_noise_x and p_noise_y) // noise x and y
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float x = ki * (float)i + kw.x * shift.x + (*p_noise_x)(i, j);
        float y = kj * (float)j + kw.y * shift.y + (*p_noise_y)(i, j);
        array(i, j) = noise_fct(x, y);
      }
}

void helper_get_noise(Array                             &array,
                      Array                             *p_noise_x,
                      Array                             *p_noise_y,
                      Vec2<float>                        shift,
                      Vec2<float>                        scale,
                      std::function<float(float, float)> noise_fct)
{
  float ai = 1.f / (float)array.shape.x * scale.x;
  float aj = 1.f / (float)array.shape.y * scale.y;

  if ((!p_noise_x) and (!p_noise_y)) // no noise
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float x = ai * (float)i + shift.x;
        float y = aj * (float)j + shift.y;
        array(i, j) = noise_fct(x, y);
      }
  else if (p_noise_x and (!p_noise_y)) // noise x only
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float x = ai * (float)i + shift.x + (*p_noise_x)(i, j);
        float y = aj * (float)j + shift.y;
        array(i, j) = noise_fct(x, y);
      }
  else if ((!p_noise_x) and p_noise_y) // noise y only
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float x = ai * (float)i + shift.x;
        float y = aj * (float)j + shift.y + (*p_noise_y)(i, j);
        array(i, j) = noise_fct(x, y);
      }
  else if (p_noise_x and p_noise_y) // noise x and y
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
      {
        float x = ai * (float)i + shift.x + (*p_noise_x)(i, j);
        float y = aj * (float)j + shift.y + (*p_noise_y)(i, j);
        array(i, j) = noise_fct(x, y);
      }
}

} // namespace hmap
