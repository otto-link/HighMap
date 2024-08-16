/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "libInterpolate/Interpolate.hpp"
#include "libInterpolate/AnyInterpolator.hpp"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/interpolate.hpp"
#include "highmap/operator.hpp"
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
                    Array             *p_stretching,
                    Vec4<float>        bbox)
{
  // create interpolator
  _2D::AnyInterpolator<
      float,
      void(std::vector<float>, std::vector<float>, std::vector<float>)>
      interp;

  if (interpolation_method == interpolator2d::DELAUNAY)
    interp = _2D::LinearDelaunayTriangleInterpolator<float>();
  else if (interpolation_method == interpolator2d::BILINEAR)
    interp = _2D::BilinearInterpolator<float>();
  else if (interpolation_method == interpolator2d::THINPLATE)
    interp = _2D::ThinPlateSplineInterpolator<float>();
  else
    LOG_ERROR("unknown interpolation method");

  // interpolate
  interp.setData(x, y, values);

  Array array_out = Array(shape);
  fill_array_using_xy_function(array_out,
                               bbox,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               [&interp](float x_, float y_, float)
                               { return interp(x_, y_); });

  return array_out;
}

} // namespace hmap
