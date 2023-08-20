/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "FastNoiseLite.h"
#include "Interpolate.hpp"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"
#include "highmap/vector.hpp"

namespace hmap
{

Array value_noise(Vec2<int>   shape,
                  Vec2<float> kw,
                  uint        seed,
                  Array      *p_noise_x,
                  Array      *p_noise_y,
                  Vec2<float> shift,
                  Vec2<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Value);

  helper_get_noise(array,
                   kw,
                   p_noise_x,
                   p_noise_y,
                   shift,
                   scale,
                   [&noise](float x, float y) { return noise.GetNoise(x, y); });
  return array;
}

Array value_noise_delaunay(Vec2<int>   shape,
                           float       kw,
                           uint        seed,
                           Array      *p_noise_x,
                           Array      *p_noise_y,
                           Vec2<float> shift,
                           Vec2<float> scale)
{
  Array array = Array({shape.x, shape.y});

  // --- Generate 'n' random grid points
  int n = (int)(kw * kw);

  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> value(n);

  random_grid(x, y, value, seed, {0.f, 1.f, 0.f, 1.f});

  if ((p_noise_x) or (p_noise_y))
    expand_grid(x, y, value, {0.f, 1.f, 0.f, 1.f});

  // --- Interpolate
  _2D::LinearDelaunayTriangleInterpolator<float> interp;
  interp.setData(x, y, value);

  // array grid
  std::vector<float> xg = linspace(shift.x, shift.x + scale.x, shape.x);
  std::vector<float> yg = linspace(shift.y, shift.y + scale.y, shape.y);

  if ((!p_noise_x) and (!p_noise_y))
  {
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
        array(i, j) = interp(xg[i], yg[j]);
  }
  else if (p_noise_x and (!p_noise_y))
  {
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
        array(i, j) = interp(xg[i] + (*p_noise_x)(i, j), yg[j]);
  }
  else if ((!p_noise_x) and p_noise_y)
  {
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
        array(i, j) = interp(xg[i], yg[j] + (*p_noise_y)(i, j));
  }
  else if (p_noise_x and p_noise_y)
  {
    for (int i = 0; i < shape.x; i++)
      for (int j = 0; j < shape.y; j++)
        array(i, j) = interp(xg[i] + (*p_noise_x)(i, j),
                             yg[j] + (*p_noise_y)(i, j));
  }

  return array;
}

Array value_noise_linear(Vec2<int>   shape,
                         Vec2<float> kw,
                         uint        seed,
                         Array      *p_noise_x,
                         Array      *p_noise_y,
                         Vec2<float> shift,
                         Vec2<float> scale)
{
  Array array = Array(shape);

  // --- Generate random values on a regular coarse grid (adjust
  // --- extent according to the input noise in order to avoid "holes"
  // --- in the data for large noise displacement)
  Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f}; // bounding box

  if (p_noise_x)
  {
    bbox.a -= 1.f;
    bbox.b += 1.f;
  }
  if (p_noise_y)
  {
    bbox.c -= 1.f;
    bbox.d += 1.f;
  }

  float lx = bbox.b - bbox.a;
  float ly = bbox.d - bbox.c;

  Vec2<float> kw_base = Vec2<float>(kw.x * lx, kw.y * ly);
  Vec2<int>   shape_base = Vec2<int>((int)kw_base.x + 1, (int)kw_base.y + 1);
  Array       values = value_noise(shape_base, kw_base, seed);

  // corresponding grids
  Array xv = Array(shape_base);
  Array yv = Array(shape_base);

  for (int i = 0; i < shape_base.x; i++)
    for (int j = 0; j < shape_base.y; j++)
    {
      xv(i, j) = bbox.a + lx * (float)i / (float)(shape_base.x - 1);
      yv(i, j) = bbox.c + ly * (float)j / (float)(shape_base.y - 1);
    }

  // --- Interpolate
  _2D::BilinearInterpolator<float> interp;
  interp.setData(xv.vector, yv.vector, values.vector);

  // array grid
  std::vector<float> xg = linspace(shift.x, shift.x + scale.x, shape.x);
  std::vector<float> yg = linspace(shift.y, shift.y + scale.y, shape.y);

  helper_get_noise(array,
                   xg,
                   yg,
                   p_noise_x,
                   p_noise_y,
                   [&interp](float x_, float y_) { return interp(x_, y_); });

  return array;
}

Array value_noise_thinplate(Vec2<int>   shape,
                            float       kw,
                            uint        seed,
                            Array      *p_noise_x,
                            Array      *p_noise_y,
                            Vec2<float> shift,
                            Vec2<float> scale)
{
  Array array = Array({shape.x, shape.y});

  // --- Generate 'n' random grid points
  int n = (int)(kw * kw);

  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> value(n);

  random_grid(x, y, value, seed, {0.f, 1.f, 0.f, 1.f});

  if ((p_noise_x) or (p_noise_y))
    expand_grid(x, y, value, {0.f, 1.f, 0.f, 1.f});

  // --- Interpolate
  _2D::ThinPlateSplineInterpolator<float> interp;
  interp.setData(x, y, value);

  // array grid
  std::vector<float> xg = linspace(shift.x, shift.x + scale.x, shape.x);
  std::vector<float> yg = linspace(shift.y, shift.y + scale.y, shape.y);

  helper_get_noise(array,
                   x,
                   y,
                   p_noise_x,
                   p_noise_y,
                   [&interp](float x_, float y_) { return interp(x_, y_); });

  return array;
}

} // namespace hmap
