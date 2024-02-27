/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "FastNoiseLite.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/interpolate.hpp"
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

  std::vector<float> x = linspace(kw.x * shift.x,
                                  kw.x * (shift.x + scale.x),
                                  array.shape.x,
                                  false);
  std::vector<float> y = linspace(kw.y * shift.y,
                                  kw.y * (shift.y + scale.y),
                                  array.shape.y,
                                  false);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               nullptr,
                               [&noise](float x_, float y_)
                               { return noise.GetNoise(x_, y_); });
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
  // generate 'n' random grid points
  int n = (int)(kw * kw);

  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> value(n);

  random_grid(x, y, value, seed, {0.f, 1.f, 0.f, 1.f});
  expand_grid(x, y, value, {0.f, 1.f, 0.f, 1.f});

  // interpolate
  Array array = interpolate2d(shape,
                              x,
                              y,
                              value,
                              interpolator2d::delaunay,
                              p_noise_x,
                              p_noise_y,
                              shift,
                              scale);

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
  // generate random values on a regular coarse grid (adjust extent
  // according to the input noise in order to avoid "holes" in the
  // data for large noise displacement)
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

  // interpolate
  Array array = interpolate2d(shape,
                              xv.vector,
                              yv.vector,
                              values.vector,
                              interpolator2d::bilinear,
                              p_noise_x,
                              p_noise_y,
                              shift,
                              scale);

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
  // generate 'n' random grid points
  int n = (int)(kw * kw);

  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> value(n);

  random_grid(x, y, value, seed, {0.f, 1.f, 0.f, 1.f});
  expand_grid(x, y, value, {0.f, 1.f, 0.f, 1.f});

  // interpolate
  Array array = interpolate2d(shape,
                              x,
                              y,
                              value,
                              interpolator2d::thinplate,
                              p_noise_x,
                              p_noise_y,
                              shift,
                              scale);

  return array;
}

} // namespace hmap
