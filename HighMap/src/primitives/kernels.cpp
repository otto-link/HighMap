/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/math.hpp"
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
  array = almost_unit_identity(array);
  return array;
}

Array cone_talus(float height, float talus)
{
  // define output array size so that starting from an amplitude h,
  // zero is indeed reached with provided slope (talus) over the
  // half-width of the domain (since we build a cone)
  int   n = std::max(1, (int)(2.f * height / talus));
  Array array = Array(Vec2<int>(n, n));

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

Array cubic_pulse_directional(Vec2<int> shape,
                              float     angle,
                              float     aspect_ratio,
                              float     anisotropy)
{
  Array array = Array(shape);

  // center and radii
  int ci = (int)(0.5f * ((float)shape.x - 1.f));
  int cj = (int)(0.5f * ((float)shape.y - 1.f));
  int ri = ci;
  int rj = cj * aspect_ratio;

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float xi = (float)i - ci;
      float yi = (float)j - cj;

      float xt = ca * xi + sa * yi;
      float yt = sa * xi - ca * yi;

      if (xt < 0.f)
        xt *= (1.f + anisotropy);

      float r = std::hypot(xt / float(ri + 1), yt / float(rj + 1));

      if (r < 1.f)
        array(i, j) = 1.f - r * r * (3.f - 2.f * r);
    }

  return array;
}

Array cubic_pulse_truncated(Vec2<int> shape, float slant_ratio, float angle)
{
  Array array = Array(shape);
  int   ri = (int)(0.5f * ((float)shape.x - 1.f));
  int   rj = (int)(0.5f * ((float)shape.y - 1.f));

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float xi = ((float)i - ri) / float(ri + 1);
      float yi = ((float)j - rj) / float(rj + 1);
      float r = std::hypot(xi, yi);

      float pulse = r < 1.f ? 1.f - r * r * (3.f - 2.f * r) : 0.f;

      float v = 1.f - (1.f / slant_ratio) * (xi * ca + yi * sa);
      float line = v < 0.f ? 0.f : (v < 1.f ? v * v * (3.f - 2.f * v) : 1.f);

      array(i, j) = std::max(0.f, line * pulse);
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

Array gabor(Vec2<int> shape, float kw, float angle)
{
  Array array = Array(shape);

  std::vector<float> x = linspace(-1.f, 1.f, array.shape.x, false);
  std::vector<float> y = linspace(-1.f, 1.f, array.shape.y, false);

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  // gaussian_decay shape approximate using a cubic pulse
  Array cpulse = cubic_pulse(shape);

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      // "kw" and not "2 kw" since the domain is [-1, 1]
      array(i, j) = cpulse(i, j) *
                    std::cos(M_PI * kw * (x[i] * ca + y[j] * sa));

  return array;
}

Array gabor_dune(Vec2<int> shape,
                 float     kw,
                 float     angle,
                 float     xtop,
                 float     xbottom)
{
  Array array = Array(shape);

  // do not start at '0' to avoid issues with modulo operator
  std::vector<float> x = linspace(1.f, 2.f, array.shape.x, false);
  std::vector<float> y = linspace(1.f, 2.f, array.shape.y, false);

  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  // gaussian_decay shape approximate using a cubic pulse
  Array cpulse = cubic_pulse(shape);

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float xp = std::fmod(kw * (x[i] * ca + y[j] * sa), 1.f);
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

      array(i, j) = cpulse(i, j) * yp;
    }

  return array;
}

Array lorentzian(Vec2<int> shape, float footprint_threshold)
{
  Array array = Array(shape);
  float cross_width = std::sqrt(1.f / (1.f / footprint_threshold - 1.f));
  float cw2 = 1.f / (cross_width * cross_width);

  for (int i = 0; i < shape.x; i++)
    for (int j = 0; j < shape.y; j++)
    {
      float x = 2.f * (float)i / (float)shape.x - 1.f;
      float y = 2.f * (float)j / (float)shape.y - 1.f;
      float r2 = x * x + y * y;
      array(i, j) = 1.f / (1.f + r2 * cw2);
    }

  return array;
}

Array lorentzian_compact(Vec2<int> shape)
{
  Array array = Array(shape);

  for (int i = 0; i < shape.x; i++)
    for (int j = 0; j < shape.y; j++)
    {
      float x = 2.f * (float)i / (float)shape.x - 1.f;
      float y = 2.f * (float)j / (float)shape.y - 1.f;
      float r2 = x * x + y * y;
      array(i, j) = r2 < 1.f ? (1.f - r2) / (1.f + 4.f * r2) : 0.f;
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

} // namespace hmap
