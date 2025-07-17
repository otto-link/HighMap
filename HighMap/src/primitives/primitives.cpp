/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/functions.hpp"
#include "highmap/geometry/grids.hpp"
#include "highmap/math.hpp"
#include "highmap/operator.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array biquad_pulse(Vec2<int>    shape,
                   float        gain,
                   const Array *p_ctrl_param,
                   const Array *p_noise_x,
                   const Array *p_noise_y,
                   const Array *p_stretching,
                   Vec2<float>  center,
                   Vec4<float>  bbox)
{
  Array          array = Array(shape);
  BiquadFunction f = BiquadFunction(gain, center);

  fill_array_using_xy_function(array,
                               bbox,
                               p_ctrl_param,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

Array bump(Vec2<int>    shape,
           float        gain,
           const Array *p_ctrl_param,
           const Array *p_noise_x,
           const Array *p_noise_y,
           const Array *p_stretching,
           Vec2<float>  center,
           Vec4<float>  bbox)
{
  Array        array = Array(shape);
  BumpFunction f = BumpFunction(gain, center);

  fill_array_using_xy_function(array,
                               bbox,
                               p_ctrl_param,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

Array constant(Vec2<int> shape, float value)
{
  Array array = Array(shape);
  for (auto &v : array.vector)
    v = value;
  return array;
}

Array disk(Vec2<int>    shape,
           float        radius,
           float        slope,
           const Array *p_ctrl_param,
           const Array *p_noise_x,
           const Array *p_noise_y,
           const Array *p_stretching,
           Vec2<float>  center,
           Vec4<float>  bbox)
{
  Array        array = Array(shape);
  DiskFunction f = DiskFunction(radius, slope, center);

  fill_array_using_xy_function(array,
                               bbox,
                               p_ctrl_param,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

Array gaussian_pulse(Vec2<int>    shape,
                     float        sigma,
                     const Array *p_ctrl_param,
                     const Array *p_noise_x,
                     const Array *p_noise_y,
                     const Array *p_stretching,
                     Vec2<float>  center,
                     Vec4<float>  bbox)
{
  Array                 array = Array(shape);
  GaussianPulseFunction f = GaussianPulseFunction(sigma, center);

  fill_array_using_xy_function(array,
                               bbox,
                               p_ctrl_param,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

Array paraboloid(Vec2<int>    shape,
                 float        angle,
                 float        a,
                 float        b,
                 float        v0,
                 bool         reverse_x,
                 bool         reverse_y,
                 const Array *p_noise_x,
                 const Array *p_noise_y,
                 const Array *p_stretching,
                 Vec2<float>  center,
                 Vec4<float>  bbox)
{
  Array array = Array(shape);

  float ca = std::cos(-angle / 180.f * M_PI);
  float sa = std::sin(-angle / 180.f * M_PI);

  float inv_a2 = (reverse_x ? -1.f : 1.f) * 1.f / (a * a);
  float inv_b2 = (reverse_y ? -1.f : 1.f) * 1.f / (b * b);

  auto lambda =
      [&ca, &sa, &v0, &inv_a2, &inv_b2, &center](float x, float y, float)
  {
    float xr = ca * (x - center.x) - sa * (y - center.y);
    float yr = sa * (x - center.x) + ca * (y - center.y);

    return inv_a2 * xr * xr + inv_b2 * yr * yr + v0;
  };

  fill_array_using_xy_function(array,
                               bbox,
                               nullptr,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               lambda);
  return array;
}

Array rectangle(Vec2<int>    shape,
                float        rx,
                float        ry,
                float        angle,
                float        slope,
                const Array *p_ctrl_param,
                const Array *p_noise_x,
                const Array *p_noise_y,
                const Array *p_stretching,
                Vec2<float>  center,
                Vec4<float>  bbox)
{
  Array             array = Array(shape);
  RectangleFunction f = RectangleFunction(rx, ry, angle, slope, center);

  fill_array_using_xy_function(array,
                               bbox,
                               p_ctrl_param,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

Array rift(Vec2<int>         shape,
           float             angle,
           float             slope,
           float             width,
           bool              sharp_bottom,
           const Array      *p_ctrl_param,
           const Array      *p_noise_x,
           const Array      *p_noise_y,
           const Array      *p_stretching,
           Vec2<float>       center,
           hmap::Vec4<float> bbox)
{
  Array              array = Array(shape);
  hmap::RiftFunction f = hmap::RiftFunction(angle,
                                            slope,
                                            width,
                                            sharp_bottom,
                                            center);

  fill_array_using_xy_function(array,
                               bbox,
                               p_ctrl_param,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

Array slope(Vec2<int>    shape,
            float        angle,
            float        slope,
            const Array *p_ctrl_param,
            const Array *p_noise_x,
            const Array *p_noise_y,
            const Array *p_stretching,
            Vec2<float>  center,
            Vec4<float>  bbox)
{
  Array               array = Array(shape);
  hmap::SlopeFunction f = hmap::SlopeFunction(angle, slope, center);

  fill_array_using_xy_function(array,
                               bbox,
                               p_ctrl_param,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

Array step(Vec2<int>         shape,
           float             angle,
           float             slope,
           const Array      *p_ctrl_param,
           const Array      *p_noise_x,
           const Array      *p_noise_y,
           const Array      *p_stretching,
           Vec2<float>       center,
           hmap::Vec4<float> bbox)
{
  Array              array = Array(shape);
  hmap::StepFunction f = hmap::StepFunction(angle, slope, center);

  fill_array_using_xy_function(array,
                               bbox,
                               p_ctrl_param,
                               p_noise_x,
                               p_noise_y,
                               p_stretching,
                               f.get_delegate());
  return array;
}

} // namespace hmap
