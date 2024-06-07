/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/features.hpp"
#include "highmap/gradient.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/math.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"
#include "highmap/selector.hpp"

namespace hmap
{

Array scan_mask(const Array &array, float contrast, float brightness)
{
  brightness *= 0.5f;
  float low = std::clamp(contrast - brightness, 0.f, 1.f);
  float high = std::clamp(contrast + brightness, 0.f, 1.f);

  contrast = contrast * 2.f - 1.f;

  Array array_out = array + contrast;

  clamp(array_out);

  array_out -= low;
  array_out *= (high - low);

  array_out = smoothstep3(array_out);

  remap(array_out, 0.f, 1.f, 0.f, 1.f);

  return array_out;
}

Array select_angle(const Array &array, float angle, float sigma, int ir)
{
  Array c = array;

  // prefiltering
  if (ir > 0)
    smooth_cpulse(c, ir);

  c = gradient_angle(c) + M_PI;
  c = select_pulse(c, angle / 180.f * M_PI, sigma / 180.f * M_PI);

  return c;
}

Array select_blob_log(const Array &array, int ir)
{
  Array c = array;
  smooth_cpulse(c, ir);
  c = -laplacian(c);
  extrapolate_borders(c, ir + 1, 0.1f);
  return c;
}

Array select_cavities(const Array &array, int ir, bool concave)
{
  Array array_smooth = array;
  smooth_cpulse(array_smooth, ir);
  Array c = curvature_mean(array_smooth);

  if (!concave)
    c *= -1.f;

  clamp_min(c, 0.f);
  return c;
}

Array select_elevation_slope(const Array &array,
                             float        gradient_scale,
                             float        vmax)
{
  Array da = gradient_norm(array) * gradient_scale * (float)array.shape.x;
  clamp_max(da, vmax);

  return pow((vmax - array) * (vmax - da), 0.5f);
}

Array select_elevation_slope(const Array &array, float gradient_scale)
{
  return select_elevation_slope(array, gradient_scale, array.max());
}

Array select_eq(const Array &array, float value)
{
  Array c = array;
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      c(i, j) = c(i, j) == value ? 1.f : 0.f;
  return c;
}

Array select_gt(const Array &array, float value)
{
  Array c = array;
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      c(i, j) = c(i, j) > value ? 1.f : 0.f;
  return c;
}

Array select_gradient_angle(const Array &array, float angle)
{
  Array c = gradient_angle(array);
  float alpha = angle / 180.f * M_PI;

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      c(i, j) = std::max(0.f, std::cos(alpha + c(i, j)));
  return c;
}

Array select_gradient_binary(const Array &array, float talus_center)
{
  Array c = gradient_norm(array);
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      c(i, j) = c(i, j) > talus_center ? 1.f : 0.f;
  return c;
}

Array select_gradient_exp(const Array &array,
                          float        talus_center,
                          float        talus_sigma)
{
  Array c = gradient_norm(array);
  c -= talus_center;
  c = exp(-c * c / (2.f * talus_sigma * talus_sigma));
  return c;
}

Array select_gradient_inv(const Array &array,
                          float        talus_center,
                          float        talus_sigma)
{
  Array c = gradient_norm(array);
  c -= talus_center;
  c = 1.f / (1.f + c * c / (talus_sigma * talus_sigma));
  return c;
}

Array select_interval(const Array &array, float value1, float value2)
{
  Array c = array;
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      if ((c(i, j) > value1) and (c(i, j) < value2))
        c(i, j) = 1.f;
      else
        c(i, j) = 0.f;
    }
  return c;
}

Array select_inward_outward_slope(const Array &array,
                                  Vec2<float>  center,
                                  Vec4<float>  bbox)
{
  Array c = Array(array.shape);

  Vec2<float> shift = {bbox.a, bbox.c};
  Vec2<float> scale = {bbox.b - bbox.a, bbox.d - bbox.c};

  int ic = (int)((center.x - shift.x) / scale.x * array.shape.x);
  int jc = (int)((center.y - shift.y) / scale.y * array.shape.y);

  for (int i = 0; i < array.shape.x - 1; i++)
    for (int j = 0; j < array.shape.y - 1; j++)
    {
      float hypot = (float)std::hypot(i - ic, j - jc);
      if (hypot > 0.f)
      {
        float u = (float)(i - ic) / hypot;
        float v = (float)(j - jc) / hypot;

        // elevation difference along the radial axis (if positive,
        // the slope is pointing to the center and is inward, otherwise the
        // slope is pointing outward)
        float dz = array.get_value_bilinear_at(i, j, u, v) - array(i, j);
        c(i, j) = dz;
      }
    }

  extrapolate_borders(c);

  return c;
}

Array select_lt(const Array &array, float value)
{
  Array c = array;
  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
      c(i, j) = c(i, j) < value ? 1.f : 0.f;
  return c;
}

Array select_pulse(const Array &array, float value, float sigma)
{
  Array c = Array(array.shape);
  float a = 1.f / sigma;
  float b = -value / sigma;

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      float r = std::abs(a * array(i, j) + b);
      if (r < 1.f)
        c(i, j) = 1.f - r * r * (3.f - 2.f * r);
    }
  return c;
}

Array select_rivers(const Array &array, float talus_ref, float clipping_ratio)
{
  // see erosion/hydraulic_stream
  Array facc = flow_accumulation_dinf(array, talus_ref);
  float vmax = clipping_ratio * std::pow(facc.sum() / (float)facc.size(), 0.5f);
  clamp(facc, 0.f, vmax);
  return facc;
}

Array select_transitions(const Array &array1,
                         const Array &array2,
                         const Array &array_blend)
{
  // set the whole mask to 1 and look for "non-transitioning" regions
  Array mask = Array(array1.shape, 1.f);

  for (int i = 0; i < array1.shape.x - 1; i++)
    for (int j = 0; j < array1.shape.y - 1; j++)
    {
      if ((array_blend(i, j) == array1(i, j)) &
          (array_blend(i + 1, j) == array1(i + 1, j)) &
          (array_blend(i, j) == array1(i, j)) &
          (array_blend(i, j + 1) == array1(i, j + 1)))
        mask(i, j) = 0.f;

      else if ((array_blend(i, j) == array2(i, j)) &
               (array_blend(i + 1, j) == array2(i + 1, j)) &
               (array_blend(i, j) == array2(i, j)) &
               (array_blend(i, j + 1) == array2(i, j + 1)))
        mask(i, j) = 0.f;
    }

  // boundaries
  for (int j = 0; j < array1.shape.y - 1; j++)
  {
    int i = array1.shape.x - 1;

    if ((array_blend(i, j) == array1(i, j)) &
        (array_blend(i - 1, j) == array1(i - 1, j)) &
        (array_blend(i, j) == array1(i, j)) &
        (array_blend(i, j + 1) == array1(i, j + 1)))
      mask(i, j) = 0.f;

    else if ((array_blend(i, j) == array2(i, j)) &
             (array_blend(i - 1, j) == array2(i - 1, j)) &
             (array_blend(i, j) == array2(i, j)) &
             (array_blend(i, j + 1) == array2(i, j + 1)))
      mask(i, j) = 0.f;
  }

  for (int i = 0; i < array1.shape.x - 1; i++)
  {
    int j = array1.shape.y - 1;

    if ((array_blend(i, j) == array1(i, j)) &
        (array_blend(i + 1, j) == array1(i + 1, j)) &
        (array_blend(i, j) == array1(i, j)) &
        (array_blend(i, j - 1) == array1(i, j - 1)))
      mask(i, j) = 0.f;

    else if ((array_blend(i, j) == array2(i, j)) &
             (array_blend(i + 1, j) == array2(i + 1, j)) &
             (array_blend(i, j) == array2(i, j)) &
             (array_blend(i, j - 1) == array2(i, j - 1)))
      mask(i, j) = 0.f;
  }

  return mask;
}

} // namespace hmap
