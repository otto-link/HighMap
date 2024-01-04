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

} // namespace hmap
