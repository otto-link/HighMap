/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/functions.hpp"
#include "highmap/math.hpp"

namespace hmap
{

// methods

FieldFunction::FieldFunction(std::unique_ptr<Function> p_base)
    : Function(), p_base(std::move(p_base))
{
  if (!this->p_base)
    throw std::invalid_argument("Base noise function must not be null.");

  xr = {0.5f};
  yr = {0.5f};
  zr = {1.f};

  this->setup_delegate();
}

FieldFunction::FieldFunction(std::unique_ptr<Function> p_base,
                             std::vector<float>        xr,
                             std::vector<float>        yr,
                             std::vector<float>        zr)
    : Function(), xr(xr), yr(yr), zr(zr), p_base(std::move(p_base))
{
  if (!this->p_base)
    throw std::invalid_argument("Base noise function must not be null.");

  this->setup_delegate();
}

void FieldFunction::setup_delegate()
{
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float value = 0.f;
        for (size_t k = 0; k < this->xr.size(); k++)
        {
          // base primitive is scaled and centered using the control points (xr,
          // yr, zr)
          float xs = (x - this->xr[k]) * this->zr[k];
          float ys = (y - this->yr[k]) * this->zr[k];

          float a = value;
          float b = this->p_base->get_value(xs, ys, ctrl_param) /
                    (1.f + this->zr[k]);

          value = a + b;
          // value = 0.5f - 2.f * (-0.5f + a) * (-0.5f + b);
          // value = a < b ? a : 2.f * b - a;
          // value = minimum_smooth(a, b, 0.01f);
          // value = std::min(a, b);
        }
        return value;
      });
}

} // namespace hmap
