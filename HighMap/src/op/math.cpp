/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <cmath>

#include "highmap/array.hpp"

namespace hmap
{

Array abs(const Array &array)
{
  Array array_out = Array(array.shape);
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [](float v) { return std::abs(v); });
  return array_out;
}

Array abs_smooth(const Array &array, float k)
{
  Array array_out = Array(array.shape);
  float k2 = k * k;
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [&k2](float v) { return std::sqrt(v * v + k2); });
  return array_out;
}

Array abs_smooth(const Array &array, float k, float vshift)
{
  Array array_out = Array(array.shape);
  float k2 = k * k;
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [&k2, &vshift](float v)
                 {
                   float vbis = v - vshift;
                   return vshift + std::sqrt(vbis * vbis + k2);
                 });
  return array_out;
}

Array abs_smooth(const Array &array, float k, const Array &vshift)
{
  Array array_out = Array(array.shape);
  float k2 = k * k;
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 vshift.vector.begin(),
                 array_out.vector.begin(),
                 [&k2](float v, float s)
                 {
                   float vbis = v - s;
                   return s + std::sqrt(vbis * vbis + k2);
                 });
  return array_out;
}

float abs_smooth(const float a, float k)
{
  float k2 = k * k;
  return std::sqrt(a * a + k2);
}

Array almost_unit_identity(const Array &array)
{
  return (2.f - array) * array * array;
}

float almost_unit_identity(const float x)
{
  return (2.f - x) * x * x;
}

float almost_unit_identity_c2(const float x)
{
  // second-order derivative equals 0 at x = 1 also to avoid
  // discontinuities in some cases
  return x * x * (x * x - 3.f * x + 3.f);
}

Array atan(const Array &array)
{
  Array array_out = Array(array.shape);
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [](float v) { return std::atan(v); });
  return array_out;
}

Array cos(const Array &array)
{
  Array array_out = Array(array.shape);
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [](float v) { return std::cos(v); });
  return array_out;
}

Array exp(const Array &array)
{
  Array array_out = Array(array.shape);
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [](float v) { return std::exp(v); });
  return array_out;
}

Array gaussian_decay(const Array &array, float sigma)
{
  float coeff = 0.5f / (sigma * sigma);
  Array array_out = Array(array.shape);
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [&coeff](float v) { return std::exp(-coeff * v * v); });
  return array_out;
}

Array hypot(const Array &array1, const Array &array2)
{
  Array array_out = Array(array1.shape);
  std::transform(array1.vector.begin(),
                 array1.vector.end(),
                 array2.vector.begin(),
                 array_out.vector.begin(),
                 [](float a, float b) { return std::hypot(a, b); });
  return array_out;
}

Array lerp(const Array &array1, const Array &array2, const Array &t)
{
  Array array_out = array1 * (1.f - t) + array2 * t;
  return array_out;
}

Array lerp(const Array &array1, const Array &array2, const float t)
{
  Array array_out = array1 * (1.f - t) + array2 * t;
  return array_out;
}

float lerp(const float a, const float b, const float t)
{
  return a * (1.f - t) + b * t;
}

Array log10(const Array &array)
{
  Array array_out = Array(array.shape);
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [](float v) { return std::log10(v); });
  return array_out;
}

Array pow(const Array &array, float exp)
{
  Array array_out = Array(array.shape);
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [&exp](float v) { return std::pow(v, exp); });
  return array_out;
}

Array sin(const Array &array)
{
  Array array_out = Array(array.shape);
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [](float v) { return std::sin(v); });
  return array_out;
}

Array smoothstep3(const Array &array, float vmin, float vmax)
{
  Array array_out = Array(array.shape);
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [&vmin, &vmax](float v)
                 {
                   if (v < vmin)
                     return vmin;
                   else if (v > vmax)
                     return vmax;
                   else
                   {
                     float vn = (v - vmin) / (vmax - vmin);
                     vn = vn * vn * (3.f - 2.f * vn);
                     return vmin + (vmax - vmin) * vn;
                   }
                 });
  return array_out;
}

float smoothstep3(const float x)
{
  return x * x * (3.f - 2.f * x);
}

Array smoothstep5(const Array &array, float vmin, float vmax)
{
  Array array_out = Array(array.shape);
  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [&vmin, &vmax](float v)
                 {
                   if (v < vmin)
                     return vmin;
                   else if (v > vmax)
                     return vmax;
                   else
                   {
                     float vn = (v - vmin) / (vmax - vmin);
                     vn = vn * vn * vn * (vn * (vn * 6.f - 15.f) + 10.f);
                     return vmin + (vmax - vmin) * vn;
                   }
                 });
  return array_out;
}

Array smoothstep5(const Array &array, const Array &vmin, const Array &vmax)
{
  Array array_out = Array(array.shape);

  for (int i = 0; i < array.shape.x; i++)
    for (int j = 0; j < array.shape.y; j++)
    {
      if (array(i, j) < vmin(i, j))
        array_out(i, j) = vmin(i, j);
      else if (array(i, j) > vmax(i, j))
        array_out(i, j) = vmax(i, j);
      else
      {
        float vn = (array(i, j) - vmin(i, j)) / (vmax(i, j) - vmin(i, j));
        vn = vn * vn * vn * (vn * (vn * 6.f - 15.f) + 10.f);
        array_out(i, j) = vmin(i, j) + (vmax(i, j) - vmin(i, j)) * vn;
      }
    }

  return array_out;
}

float smoothstep5(const float x)
{
  return x * x * x * (x * (x * 6.f - 15.f) + 10.f);
}

} // namespace hmap
