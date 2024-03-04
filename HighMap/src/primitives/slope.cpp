/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/noise_function.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"
#include "highmap/vector.hpp"

namespace hmap
{

// --- (x, y) function definitions

SlopeFunction::SlopeFunction(float angle, float slope, Vec2<float> center)
    : NoiseFunction(), slope(slope), center(center)
{
  this->set_angle(angle);

  this->function = [this](float x, float y, float)
  {
    float r = this->ca * (x - this->center.x) + this->sa * (y - this->center.y);
    return this->slope * r;
  };
}

// --- wrappers for array filling

Array slope(Vec2<int>   shape,
            float       angle,
            float       slope,
            Array      *p_noise_x,
            Array      *p_noise_y,
            Array      *p_stretching,
            Vec2<float> center,
            Vec2<float> shift,
            Vec2<float> scale)
{
  Array               array = Array(shape);
  hmap::SlopeFunction f = hmap::SlopeFunction(angle, slope, center);
  std::vector<float>  x, y;
  hmap::Vec4<float>   bbox = {0.f + shift.x,
                              scale.x + shift.x,
                              0.f + shift.y,
                              scale.y + shift.y};

  grid_xy_vector(x, y, shape, bbox);

  fill_array_using_xy_function(array,
                               x,
                               y,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_function());
  return array;
}

} // namespace hmap
