/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void alter_elevation(Array      &array,
                     Cloud      &cloud,
                     int         ir,
                     float       footprint_ratio,
                     Vec2<float> shift,
                     Vec2<float> scale)
{
  Array amp = Array(array.shape);
  for (auto &p : cloud.points)
  {
    int ic = (int)((p.x - shift.x) / scale.x * array.shape.x);
    int jc = (int)((p.y - shift.y) / scale.y * array.shape.y);

    // kernel size
    int   nk = (int)((2 * ir + 1) * (1.f + footprint_ratio * std::abs(p.v)));
    Array kernel = cubic_pulse(Vec2(nk, nk));

    // --
    // truncate kernel to make it fit into the heightmap array
    int nk_i0 = (int)(std::floor(0.5f * kernel.shape.x)); // left
    int nk_i1 = kernel.shape.x - nk_i0;                   // right
    int nk_j0 = (int)(std::floor(0.5f * kernel.shape.y));
    int nk_j1 = kernel.shape.y - nk_j0;

    int ik0 = std::max(0, nk_i0 - ic);
    int jk0 = std::max(0, nk_j0 - jc);
    int ik1 = std::min(kernel.shape.x,
                       kernel.shape.x - (ic + nk_i1 - amp.shape.x));
    int jk1 = std::min(kernel.shape.y,
                       kernel.shape.y - (jc + nk_j1 - amp.shape.y));

    // where it goes in the array
    int i0 = std::max(ic - nk_i0, 0);
    int j0 = std::max(jc - nk_j0, 0);
    // int i1 = std::min(ic + nk_i1, amp.shape.x);
    // int j1 = std::min(jc + nk_j1, amp.shape.y);

    float sign = (array(ic, jc) > 0.f) - (array(ic, jc) < 0.f);

    for (int i = ik0; i < ik1; i++)
      for (int j = jk0; j < jk1; j++)
        amp(i - ik0 + i0, j - jk0 + j0) += p.v * kernel(i, j) *
                                           array(i - ik0 + i0, j - jk0 + j0) *
                                           sign;
  }

  array += amp;
}

void flip_lr(Array &array)
{
  for (int i = 0; i < (int)(0.5f * array.shape.y); i++)
    for (int j = 0; j < array.shape.y; j++)
      std::swap(array(i, j), array(array.shape.x - i - 1, j));
}

void flip_ud(Array &array)
{
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < (int)(0.5f * array.shape.y); j++)
      std::swap(array(i, j), array(i, array.shape.y - j - 1));
}

void normal_displacement(Array &array, float amount, int ir, bool reverse)
{
  Array array_f = array;
  Array array_new = Array(array.shape);

  if (ir > 0)
    smooth_cpulse(array_f, ir);

  // add a shape factor to avoid artifacts close to the boundaries
  Array factor = smooth_cosine(array.shape);

  if (reverse)
    amount = -amount;

  for (int i = 1; i < array.shape.x - 1; i++)
    for (int j = 1; j < array.shape.y - 1; j++)
    {
      Vec3<float> n = array_f.get_normal_at(i, j);

      float x = (float)i - amount * array.shape.x * n.x * factor(i, j);
      float y = (float)j - amount * array.shape.y * n.y * factor(i, j);

      // bilinear interpolation parameters
      int ip = std::clamp((int)x, 0, array.shape.x - 1);
      int jp = std::clamp((int)y, 0, array.shape.y - 1);

      float u = std::clamp(x - (float)ip, 0.f, 1.f);
      float v = std::clamp(y - (float)jp, 0.f, 1.f);

      array_new(i, j) = array.get_value_bilinear_at(ip, jp, u, v);
    }
  fill_borders(array_new);

  array = array_new;
}

void normal_displacement(Array &array,
                         Array *p_mask,
                         float  amount,
                         int    ir,
                         bool   reverse)
{
  if (!p_mask)
    normal_displacement(array, amount, ir, reverse);
  else
  {
    Array array_f = array;
    normal_displacement(array_f, amount, ir, reverse);
    array = lerp(array, array_f, *(p_mask));
  }
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

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
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
                Array       *p_noise_x,
                Array       *p_noise_y,
                Vec4<float>  bbox)
{
  hmap::ArrayFunction f = hmap::ArrayFunction(array,
                                              Vec2<float>(1.f, 1.f),
                                              periodic);

  Array dx_array = constant(array.shape, -dx);
  Array dy_array = constant(array.shape, -dy);

  if (p_noise_x)
    dx_array += *p_noise_x;

  if (p_noise_y)
    dy_array += *p_noise_y;

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

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      array_out(j, i) = array(i, j);

  return array_out;
}

Array zoom(const Array &array,
           float        zoom_factor,
           bool         periodic,
           Vec2<float>  center,
           Array       *p_noise_x,
           Array       *p_noise_y,
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
