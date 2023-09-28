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
