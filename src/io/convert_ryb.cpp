/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"

namespace hmap
{

void convert_rgb_to_ryb(Array &r,
                        Array &g,
                        Array &b,
                        Array &r_out,
                        Array &y_out,
                        Array &b_out)
{
  // http://nishitalab.org/user/UEI/publication/Sugita_IWAIT2015.pdf
  for (int i = 0; i < r.shape.x; i++)
    for (int j = 0; j < r.shape.y; j++)
    {
      // remove white component
      float iw = std::min(std::min(r(i, j), g(i, j)), b(i, j));

      float r_s = r(i, j) - iw;
      float g_s = g(i, j) - iw;
      float b_s = b(i, j) - iw;

      // to ryb
      float gb_s_min = std::min(r_s, g_s);
      float rc_s = r_s - gb_s_min;
      float yc_s = 0.5f * (g_s + gb_s_min);
      float bc_s = 0.5f * (b_s + g_s - gb_s_min);

      // normalize
      float n = std::max(std::max(rc_s, yc_s), bc_s) /
                std::max(std::max(r_s, g_s), b_s);
      rc_s /= n;
      yc_s /= n;
      bc_s /= n;

      // add black component
      float ib = std::min(std::min(1.f - r(i, j), 1.f - g(i, j)),
                          1.f - b(i, j));

      r_out(i, j) = rc_s + ib;
      y_out(i, j) = yc_s + ib;
      b_out(i, j) = bc_s + ib;
    }
}

void convert_ryb_to_rgb(Array &r,
                        Array &y,
                        Array &b,
                        Array &r_out,
                        Array &g_out,
                        Array &b_out)
{
  // http://nishitalab.org/user/UEI/publication/Sugita_IWAIT2015.pdf
  for (int i = 0; i < r.shape.x; i++)
    for (int j = 0; j < r.shape.y; j++)
    {
      // remove white component
      float iw = std::min(std::min(r(i, j), y(i, j)), b(i, j));

      float r_s = r(i, j) - iw;
      float y_s = y(i, j) - iw;
      float b_s = b(i, j) - iw;

      // to rgb
      float yb_s_min = std::min(y_s, b_s);
      float rc_s = r_s + y_s - yb_s_min;
      float gc_s = y_s + 2.f * yb_s_min;
      float bc_s = 2.f * (b_s - yb_s_min);

      // normalize
      float n = std::max(std::max(rc_s, gc_s), bc_s) /
                std::max(std::max(r_s, y_s), b_s);
      rc_s /= n;
      gc_s /= n;
      bc_s /= n;

      // add black component
      float ib = std::min(std::min(1.f - r(i, j), 1.f - y(i, j)),
                          1.f - b(i, j));

      r_out(i, j) = rc_s + ib;
      g_out(i, j) = gc_s + ib;
      b_out(i, j) = bc_s + ib;
    }
}

} // namespace hmap
