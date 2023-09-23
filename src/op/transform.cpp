/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/kernels.hpp"
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

void normal_displacement(Array &array, float amount, int ir, bool reverse)
{
  Array array_f = array;
  Array array_new = Array(array.shape);

  if (ir > 0)
    smooth_cpulse(array_f, ir);

  if (reverse)
    amount = -amount;

  for (int i = 1; i < array.shape.x - 1; i++)
    for (int j = 1; j < array.shape.y - 1; j++)
    {
      Vec3<float> n = array_f.get_normal_at(i, j);

      float x = (float)i - amount * array.shape.x * n.x;
      float y = (float)j - amount * array.shape.y * n.y;

      // bilinear interpolation parameters
      int ip = std::clamp((int)x, 0, array.shape.x - 1);
      int jp = std::clamp((int)y, 0, array.shape.y - 1);

      float u = std::clamp(x - (float)ip, 0.f, 1.f);
      float v = std::clamp(y - (float)jp, 0.f, 1.f);

      array_new(i, j) = array.get_value_bilinear_at(ip, jp, u, v);
    }
  extrapolate_borders(array_new);

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

} // namespace hmap
