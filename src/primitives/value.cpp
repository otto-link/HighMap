/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "FastNoiseLite.h"
#include "Interpolate.hpp"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array value_noise(std::vector<int>   shape,
                  std::vector<float> kw,
                  uint               seed,
                  Array             *p_noise_x,
                  Array             *p_noise_y,
                  std::vector<float> shift,
                  std::vector<float> scale)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Value);

  array = helper_get_noise(array,
                           kw,
                           p_noise_x,
                           p_noise_y,
                           shift,
                           scale,
                           [&noise](float x, float y)
                           { return noise.GetNoise(x, y); });
  return array;
}

Array value_noise_delaunay(std::vector<int>   shape,
                           float              kw,
                           uint               seed,
                           Array             *p_noise_x,
                           Array             *p_noise_y,
                           std::vector<float> shift,
                           std::vector<float> scale)
{
  Array array = Array(shape);

  // --- Generate 'n' random grid points
  int n = (int)(kw * kw);

  std::vector<float> x(n);
  std::vector<float> y(n);
  std::vector<float> value(n);

  random_grid(x, y, value, seed, {0.f, 1.f, 0.f, 1.f});
  expand_grid(x, y, value, {0.f, 1.f, 0.f, 1.f});

  // --- Interpolate
  _2D::LinearDelaunayTriangleInterpolator<float> interp;
  interp.setData(x, y, value);

  // array grid
  std::vector<float> xg = linspace(shift[0], shift[0] + scale[0], shape[0]);
  std::vector<float> yg = linspace(shift[1], shift[1] + scale[1], shape[1]);

  if ((!p_noise_x) and (!p_noise_y))
  {
    for (int i = 0; i < shape[0]; i++)
      for (int j = 0; j < shape[1]; j++)
        array(i, j) = interp(xg[i], yg[j]);
  }
  else if (p_noise_x and (!p_noise_y))
  {
    for (int i = 0; i < shape[0]; i++)
      for (int j = 0; j < shape[1]; j++)
        array(i, j) = interp(xg[i] + (*p_noise_x)(i, j), yg[j]);
  }
  else if ((!p_noise_x) and p_noise_y)
  {
    for (int i = 0; i < shape[0]; i++)
      for (int j = 0; j < shape[1]; j++)
        array(i, j) = interp(xg[i], yg[j] + (*p_noise_y)(i, j));
  }
  else if (p_noise_x and p_noise_y)
  {
    for (int i = 0; i < shape[0]; i++)
      for (int j = 0; j < shape[1]; j++)
        array(i, j) = interp(xg[i] + (*p_noise_x)(i, j),
                             yg[j] + (*p_noise_y)(i, j));
  }

  return array;
}

Array value_noise_linear(std::vector<int>   shape,
                         std::vector<float> kw,
                         uint               seed,
                         Array             *p_noise_x,
                         Array             *p_noise_y,
                         std::vector<float> shift,
                         std::vector<float> scale)
{
  Array array = Array(shape);

  // --- Generate random values on a regular coarse grid
  std::vector<int> shape_base = {(int)kw[0] + 1, (int)kw[1] + 1};
  Array            values = value_noise(shape_base, kw, seed);

  // corresponding grids
  Array xv = Array(shape_base);
  Array yv = Array(shape_base);

  for (int i = 0; i < shape_base[0]; i++)
    for (int j = 0; j < shape_base[1]; j++)
    {
      xv(i, j) = (float)i / (float)(shape_base[0] - 1);
      yv(i, j) = (float)j / (float)(shape_base[1] - 1);
    }

  // --- Interpolate
  _2D::BilinearInterpolator<float> interp;
  interp.setData(xv.vector, yv.vector, values.vector);

  // array grid
  std::vector<float> xg = linspace(shift[0], shift[0] + scale[0], shape[0]);
  std::vector<float> yg = linspace(shift[1], shift[1] + scale[1], shape[1]);

  if ((!p_noise_x) and (!p_noise_y))
  {
    for (int i = 0; i < shape[0]; i++)
      for (int j = 0; j < shape[1]; j++)
        array(i, j) = interp(xg[i], yg[j]);
  }
  else if (p_noise_x and (!p_noise_y))
  {
    for (int i = 0; i < shape[0]; i++)
      for (int j = 0; j < shape[1]; j++)
        array(i, j) = interp(xg[i] + (*p_noise_x)(i, j), yg[j]);
  }
  else if ((!p_noise_x) and p_noise_y)
  {
    for (int i = 0; i < shape[0]; i++)
      for (int j = 0; j < shape[1]; j++)
        array(i, j) = interp(xg[i], yg[j] + (*p_noise_y)(i, j));
  }
  else if (p_noise_x and p_noise_y)
  {
    for (int i = 0; i < shape[0]; i++)
      for (int j = 0; j < shape[1]; j++)
        array(i, j) = interp(xg[i] + (*p_noise_x)(i, j),
                             yg[j] + (*p_noise_y)(i, j));
  }

  return array;
}

} // namespace hmap
