/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/filters.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"
#include "highmap/transform.hpp"

namespace hmap
{

void flip_lr(Array &array)
{
  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < (int)(0.5f * array.shape.y); i++)
      std::swap(array(i, j), array(array.shape.x - i - 1, j));
}

void flip_ud(Array &array)
{
  for (int j = 0; j < (int)(0.5f * array.shape.y); j++)
    for (int i = 0; i < array.shape.x; i++)
      std::swap(array(i, j), array(i, array.shape.y - j - 1));
}

void rot180(Array &array)
{
  flip_lr(array);
  flip_ud(array);
}

void rot270(Array &array)
{
  array = transpose(array);
  flip_lr(array);
}

void rot90(Array &array)
{
  array = transpose(array);
  flip_ud(array);
}

void rotate(Array &array, float angle, bool zero_padding)
{
  float ca = std::cos(angle / 180.f * M_PI);
  float sa = std::sin(angle / 180.f * M_PI);

  // create a larger array filled using symmetry to have a domain
  // large enough to avoid 'holes' while interpolating
  int nbuffer = std::max((int)(0.25f * array.shape.x),
                         (int)(0.25f * array.shape.y));
  nbuffer = std::max(1, nbuffer);

  Array array_bf = generate_buffered_array(
      array,
      Vec4<int>(nbuffer, nbuffer, nbuffer, nbuffer),
      zero_padding);

  float xc = 0.5f * array.shape.x;
  float yc = 0.5f * array.shape.y;

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float x = xc + ca * (i - xc) - sa * (j - yc);
      float y = yc + sa * (i - xc) + ca * (j - yc);

      // corresponding nearest cells in buffered array (and bilinear
      // interpolation parameters)
      int ix = std::clamp((int)x, 0, array.shape.x - 1);
      int jy = std::clamp((int)y, 0, array.shape.y - 1);

      float u = std::clamp(x - (float)ix, 0.f, 1.f);
      float v = std::clamp(y - (float)jy, 0.f, 1.f);

      int ib = nbuffer + ix;
      int jb = nbuffer + jy;

      array(i, j) = array_bf.get_value_bilinear_at(ib, jb, u, v);
    }
}

Array translate(const Array &array,
                float        dx,
                float        dy,
                bool         periodic,
                const Array *p_noise_x,
                const Array *p_noise_y,
                Vec4<float>  bbox)
{
  hmap::ArrayFunction f = hmap::ArrayFunction(array,
                                              Vec2<float>(1.f, 1.f),
                                              periodic);

  Array dx_array = constant(array.shape, -dx);
  Array dy_array = constant(array.shape, -dy);

  if (p_noise_x) dx_array += *p_noise_x;

  if (p_noise_y) dy_array += *p_noise_y;

  Array array_out = Array(array.shape);

  fill_array_using_xy_function(array_out,
                               bbox,
                               nullptr,
                               &dx_array,
                               &dy_array,
                               nullptr,
                               f.get_delegate());

  return array_out;
}

Array transpose(const Array &array)
{
  Array array_out = Array(Vec2<int>(array.shape.y, array.shape.x));

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      array_out(j, i) = array(i, j);

  return array_out;
}

Array zoom(const Array &array,
           float        zoom_factor,
           bool         periodic,
           Vec2<float>  center,
           const Array *p_noise_x,
           const Array *p_noise_y,
           Vec4<float>  bbox)
{

  hmap::ArrayFunction f = hmap::ArrayFunction(
      array,
      Vec2<float>(1.f / zoom_factor, 1.f / zoom_factor),
      periodic);

  Array array_out = Array(array.shape);

  Vec4<float> bbox2 = {bbox.a + center.x,
                       bbox.b + center.x,
                       bbox.c + center.y,
                       bbox.d + center.y};

  fill_array_using_xy_function(array_out,
                               bbox2,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               nullptr,
                               f.get_delegate());

  return array_out;
}

} // namespace hmap
