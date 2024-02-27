/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "Interpolate.hpp"
#include "AnyInterpolator.hpp"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/interpolate.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array interpolate2d(Vec2<int>          shape,
                    std::vector<float> x,
                    std::vector<float> y,
                    std::vector<float> values,
                    int                interpolation_method,
                    Array             *p_noise_x,
                    Array             *p_noise_y,
                    Vec2<float>        shift,
                    Vec2<float>        scale)
{
  // create interpolator
  _2D::AnyInterpolator<
      float,
      void(std::vector<float>, std::vector<float>, std::vector<float>)>
      interp;

  if (interpolation_method == interpolator2d::delaunay)
    interp = _2D::LinearDelaunayTriangleInterpolator<float>();
  else if (interpolation_method == interpolator2d::bilinear)
    interp = _2D::BilinearInterpolator<float>();
  else if (interpolation_method == interpolator2d::thinplate)
    interp = _2D::ThinPlateSplineInterpolator<float>();
  else
    LOG_ERROR("unknown interpolation method");

  // interpolate
  interp.setData(x, y, values);

  // array grid
  std::vector<float> xi = linspace(shift.x, shift.x + scale.x, shape.x, false);
  std::vector<float> yi = linspace(shift.y, shift.y + scale.y, shape.y, false);

  Array array_out = Array(shape);
  helper_get_noise(array_out,
                   xi,
                   yi,
                   p_noise_x,
                   p_noise_y,
                   nullptr,
                   [&interp](float x_, float y_) { return interp(x_, y_); });

  return array_out;
}

} // namespace hmap
