/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/export.hpp"
#include "highmap/operator.hpp"

namespace hmap
{

void export_splatmap_png_16bit(std::string fname,
                               Array      *p_r,
                               Array      *p_g,
                               Array      *p_b,
                               Array      *p_a)
{
  std::vector<uint16_t> img(p_r->shape.x * p_r->shape.y * 4);

  float vmin;
  float vmax;

  float a_r = 0.f;
  float b_r = 0.f;
  float a_g = 0.f;
  float b_g = 0.f;
  float a_b = 0.f;
  float b_b = 0.f;
  float a_a = 0.f;
  float b_a = 0.f;

  vmin = p_r->min();
  vmax = p_r->max();
  if (vmin != vmax)
  {
    a_r = 65535.f / (vmax - vmin);
    b_r = -65535.f * vmin / (vmax - vmin);
  }

  if (p_g)
  {
    vmin = p_g->min();
    vmax = p_g->max();
    if (vmin != vmax)
    {
      a_g = 65535.f / (vmax - vmin);
      b_g = -65535.f * vmin / (vmax - vmin);
    }
  }

  if (p_b)
  {
    vmin = p_b->min();
    vmax = p_b->max();
    if (vmin != vmax)
    {
      a_b = 65535.f / (vmax - vmin);
      b_b = -65535.f * vmin / (vmax - vmin);
    }
  }

  if (p_a)
  {
    vmin = p_a->min();
    vmax = p_a->max();
    if (vmin != vmax)
    {
      a_a = 65535.f / (vmax - vmin);
      b_a = -65535.f * vmin / (vmax - vmin);
    }
  }

  int k = 0;
  for (int j = p_r->shape.y - 1; j > -1; j -= 1)
    for (int i = 0; i < p_r->shape.x; i++)
    {
      img[k++] = (uint16_t)(a_r * (*p_r)(i, j) + b_r);

      if (p_g)
        img[k++] = (uint16_t)(a_g * (*p_g)(i, j) + b_g);
      else
        img[k++] = 0;

      if (p_b)
        img[k++] = (uint16_t)(a_b * (*p_b)(i, j) + b_b);
      else
        img[k++] = 0;

      if (p_a)
        img[k++] = (uint16_t)(a_a * (*p_a)(i, j) + b_a);
      else
        img[k++] = 0;
    }
  write_png_rgba_16bit(fname, img, p_r->shape);
}

} // namespace hmap
