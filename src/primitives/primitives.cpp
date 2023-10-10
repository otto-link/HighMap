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

Array base_elevation(Vec2<int>                       shape,
                     std::vector<std::vector<float>> values,
                     float                           width_factor,
                     Array                          *p_noise_x,
                     Array                          *p_noise_y,
                     Vec2<float>                     shift,
                     Vec2<float>                     scale)
{
  Array array = Array(shape);

  // get number of control points per direction
  size_t ni = values.size();
  size_t nj = values[0].size();

  // corresponding coordinates
  float              dxc = 1.f / (float)ni;
  float              dyc = 1.f / (float)nj;
  std::vector<float> xc = linspace(0.5f * dxc, 1.f - 0.5f * dxc, ni);
  std::vector<float> yc = linspace(0.5f * dyc, 1.f - 0.5f * dyc, nj);

  std::vector<float> x = linspace(shift.x, shift.x + scale.x, shape.x, false);
  std::vector<float> y = linspace(shift.y, shift.y + scale.y, shape.y, false);

  // Gaussian half-widths based on the mesh discretization
  float s2x = 2.f / dxc / dxc / width_factor;
  float s2y = 2.f / dyc / dyc / width_factor;

  for (size_t p = 0; p < ni; p++)
    for (size_t q = 0; q < nj; q++)
    {
      auto lambda = [&xc, &yc, &p, &q, &s2x, &s2y](float x_, float y_)
      {
        float r2 = s2x * (x_ - xc[p]) * (x_ - xc[p]) +
                   s2y * (y_ - yc[q]) * (y_ - yc[q]);
        return std::exp(-r2);
      };
      array += values[p][q] *
               helper_get_noise(x, y, p_noise_x, p_noise_y, lambda);
    }

  return array;
}

Array bump(Vec2<int>   shape,
           float       gain,
           Array      *p_noise_x,
           Array      *p_noise_y,
           Vec2<float> shift,
           Vec2<float> scale)
{
  Array z = Array(shape);

  std::vector<float> x =
      linspace(shift.x - 0.5f, shift.x + scale.x - 0.5f, shape.x, false);
  std::vector<float> y =
      linspace(shift.y - 0.5f, shift.y + scale.y - 0.5f, shape.y, false);

  float gain_inv = 1.f / gain;

  auto lambda = [&gain_inv](float x_, float y_)
  {
    float r2 = x_ * x_ + y_ * y_;
    return r2 > 0.25f ? 0.f
                      : std::pow(std::exp(-1.f / (1.f - 4.f * r2)), gain_inv);
  };

  helper_get_noise(z, x, y, p_noise_x, p_noise_y, lambda);

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
  std::vector<float> x =
      linspace(0.f + shift.x, scale.x + shift.x, shape.x, false);
  std::vector<float> y =
      linspace(0.f + shift.y, scale.y + shift.y, shape.y, false);

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw, &phase_shift, &xtop, &xbottom](float x)
  {
    float xp = std::fmod(kw * x + phase_shift, 1.f);
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
  std::vector<float> x =
      linspace(0.f + shift.x, scale.x + shift.x, shape.x, false);
  std::vector<float> y =
      linspace(0.f + shift.y, scale.y + shift.y, shape.y, false);
  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw, &phase_shift](float x)
  { return std::cos(2.f * M_PI * kw * x + phase_shift); };

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
                  float       phase_shift,
                  Array      *p_noise,
                  Vec2<float> shift,
                  Vec2<float> scale)
{
  Array              array = Array(shape);
  std::vector<float> x =
      linspace(0.f + shift.x, scale.x + shift.x, shape.x, false);
  std::vector<float> y =
      linspace(0.f + shift.y, scale.y + shift.y, shape.y, false);
  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  auto lambda = [&kw](float x)
  { return x = 2.f * (int)(kw * x) - (int)(2.f * kw * x) + 1.f; };

  if (p_noise != nullptr)
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j) +
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
  std::vector<float> x =
      linspace(0.f + shift.x, scale.x + shift.x, shape.x, false);
  std::vector<float> y =
      linspace(0.f + shift.y, scale.y + shift.y, shape.y, false);
  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

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
        array(i, j) = lambda(ca * x[i] + sa * y[j] + (*p_noise)(i, j) +
                             phase_shift);
  else
    for (int i = 0; i < array.shape.x; i++)
      for (int j = 0; j < array.shape.y; j++)
        array(i, j) = lambda(ca * x[i] + sa * y[j] + phase_shift);

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
                      std::vector<float>                &x,
                      std::vector<float>                &y,
                      Array                             *p_noise_x,
                      Array                             *p_noise_y,
                      Array                             *p_stretching,
                      std::function<float(float, float)> noise_fct)
{
  Vec2<int> shape = array.shape;

  if (p_stretching) // with stretching
  {
    if ((!p_noise_x) and (!p_noise_y))
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i, j) = noise_fct(x[i] * (*p_stretching)(i, j),
                                  y[j] * (*p_stretching)(i, j));
    }
    else if (p_noise_x and (!p_noise_y))
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i, j) = noise_fct(x[i] * (*p_stretching)(i, j) +
                                      (*p_noise_x)(i, j),
                                  y[j] * (*p_stretching)(i, j));
    }
    else if ((!p_noise_x) and p_noise_y)
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i, j) = noise_fct(x[i] * (*p_stretching)(i, j),
                                  y[j] * (*p_stretching)(i, j) +
                                      (*p_noise_y)(i, j));
    }
    else if (p_noise_x and p_noise_y)
    {
      for (int i = 0; i < shape.x; i++)
        for (int j = 0; j < shape.y; j++)
          array(i, j) = noise_fct(
              x[i] * (*p_stretching)(i, j) + (*p_noise_x)(i, j),
              y[j] * (*p_stretching)(i, j) + (*p_noise_y)(i, j));
    }
  }
  else // without stretching
  {
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
}

Array helper_get_noise(std::vector<float>                &x,
                       std::vector<float>                &y,
                       Array                             *p_noise_x,
                       Array                             *p_noise_y,
                       std::function<float(float, float)> noise_fct)
{
  Vec2<int> shape = {(int)x.size(), (int)y.size()};
  Array     array = Array(shape);

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

  return array;
}

} // namespace hmap
