/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"
#include "highmap/curvature.hpp"
#include "highmap/filters.hpp"
#include "highmap/gradient.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/math.hpp"
#include "highmap/morphology.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"
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
  if (ir > 0) smooth_cpulse(c, ir);

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

  if (!concave) c *= -1.f;

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
  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      c(i, j) = c(i, j) == value ? 1.f : 0.f;
  return c;
}

Array select_gt(const Array &array, float value)
{
  Array c = array;
  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      c(i, j) = c(i, j) > value ? 1.f : 0.f;
  return c;
}

Array select_gradient_angle(const Array &array, float angle)
{
  Array c = gradient_angle(array);
  float alpha = angle / 180.f * M_PI;

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      c(i, j) = std::max(0.f, std::cos(alpha + c(i, j)));
  return c;
}

Array select_gradient_binary(const Array &array, float talus_center)
{
  Array c = gradient_norm(array);
  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
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
  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
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

  for (int j = 0; j < array.shape.y - 1; j++)
    for (int i = 0; i < array.shape.x - 1; i++)
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
  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
      c(i, j) = c(i, j) < value ? 1.f : 0.f;
  return c;
}

Array select_midrange(const Array &array, float gain, float vmin, float vmax)
{
  Array c = array;
  hmap::remap(c, -1.f, 1.f, vmin, vmax);

  float norm_coeff = 1.f / std::exp(-1.f);

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float v = c(i, j) * c(i, j);
      c(i, j) = std::pow(norm_coeff * std::exp(-1.f / (1.f - v)), 1.f / gain);
    }
  return c;
}

Array select_midrange(const Array &array, float gain)
{
  return select_midrange(array, gain, array.min(), array.max());
}

void select_multiband3(const Array &array,
                       Array       &band_low,
                       Array       &band_mid,
                       Array       &band_high,
                       float        ratio1,
                       float        ratio2,
                       float        overlap,
                       float        vmin,
                       float        vmax)
{
  band_low = Array(array.shape);
  band_mid = Array(array.shape);
  band_high = Array(array.shape);

  float v1 = vmin + ratio1 * (vmax - vmin);
  float v2 = vmin + ratio2 * (vmax - vmin);

  auto lambda = [](float r, float r0, float r1, float r2, float overlap)
  {
    float w0 = overlap * (r1 - r0);
    float w2 = overlap * (r2 - r1);

    if (r < r0 - w0) return 0.f;
    if (r > r2 + w2) return 0.f;

    if (r > r0 + w0 && r < r2 - w2) return 1.f;

    float rn = 0.f;
    if (r < r0 + w0) rn = (r - r0 + w0) / 2.f / w0;
    if (r > r2 - w2) rn = 1.f - (r - r2 + w2) / 2.f / w2;
    return rn * rn * (3.f - 2.f * rn);
  };

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      band_low(i,
               j) = lambda(array(i, j), vmin, 0.5f * (vmin + v1), v1, overlap);
      band_mid(i, j) = lambda(array(i, j), v1, 0.5f * (v1 + v2), v2, overlap);
      band_high(i,
                j) = lambda(array(i, j), v2, 0.5f * (v2 + vmax), vmax, overlap);
    }
}

void select_multiband3(const Array &array,
                       Array       &band_low,
                       Array       &band_mid,
                       Array       &band_high,
                       float        ratio1,
                       float        ratio2,
                       float        overlap)
{
  float vmin = array.min();
  float vmax = array.max();

  select_multiband3(array,
                    band_low,
                    band_mid,
                    band_high,
                    ratio1,
                    ratio2,
                    overlap,
                    vmin,
                    vmax);
}

Array select_pulse(const Array &array, float value, float sigma)
{
  Array c = Array(array.shape);
  float a = 1.f / sigma;
  float b = -value / sigma;

  for (int j = 0; j < array.shape.y; j++)
    for (int i = 0; i < array.shape.x; i++)
    {
      float r = std::abs(a * array(i, j) + b);
      if (r < 1.f) c(i, j) = 1.f - r * r * (3.f - 2.f * r);
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

  for (int j = 0; j < array1.shape.y - 1; j++)
    for (int i = 0; i < array1.shape.x - 1; i++)
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

Array select_valley(const Array &z,
                    int          ir,
                    bool         zero_at_borders,
                    bool         ridge_select)
{
  Array w = z;
  smooth_cpulse(w, std::max(1, ir));

  if (not(ridge_select)) w *= -1.f;

  w = curvature_mean(w);
  make_binary(w);
  w = relative_distance_from_skeleton(w, ir, zero_at_borders);

  return w;
}

} // namespace hmap
