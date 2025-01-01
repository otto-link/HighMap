/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <cmath>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/convolve.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void chop(Array &array, float vmin)
{
  auto lambda = [&vmin](float x) { return x > vmin ? x : 0.f; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void chop_max_smooth(Array &array, float vmax)
{
  auto lambda = [&vmax](float x)
  {
    if (x > vmax)
      x = 0.f;
    else if (x > 0.5f * vmax)
      x = vmax - x;
    return x;
  };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void clamp(Array &array, float vmin, float vmax)
{
  auto lambda = [&vmin, &vmax](float x) { return std::clamp(x, vmin, vmax); };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void clamp_max(Array &array, float vmax)
{
  auto lambda = [&vmax](float x) { return x < vmax ? x : vmax; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void clamp_max(Array &array, const Array &vmax)
{
  auto lambda = [](float x, float vmax) { return x < vmax ? x : vmax; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 vmax.vector.begin(),
                 array.vector.begin(),
                 lambda);
}

void clamp_max_smooth(Array &array, float vmax, float k)
{
  auto lambda = [&k, &vmax](float x)
  {
    float h = std::max(k - std::abs(x - vmax), 0.f) / k;
    return std::min(x, vmax) - std::pow(h, 3) * k / 6.f;
  };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void clamp_max_smooth(Array &array, const Array &vmax, float k)
{
  auto lambda = [&k](float x, float vmax)
  {
    float h = std::max(k - std::abs(x - vmax), 0.f) / k;
    return std::min(x, vmax) - std::pow(h, 3) * k / 6.f;
  };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 vmax.vector.begin(),
                 array.vector.begin(),
                 lambda);
}

void clamp_min(Array &array, float vmin)
{
  auto lambda = [&vmin](float x) { return x > vmin ? x : vmin; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void clamp_min(Array &array, const Array &vmin)
{
  auto lambda = [](float x, float vmin) { return x > vmin ? x : vmin; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 vmin.vector.begin(),
                 array.vector.begin(),
                 lambda);
}

void clamp_min_smooth(Array &array, float vmin, float k)
{
  auto lambda = [&k, &vmin](float x)
  {
    float h = std::max(k - std::abs(x - vmin), 0.f) / k;
    return std::max(x, vmin) + std::pow(h, 3) * k / 6.f;
  };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void clamp_min_smooth(Array &array, const Array &vmin, float k)
{
  auto lambda = [&k](float x, float vmin)
  {
    float h = std::max(k - std::abs(x - vmin), 0.f) / k;
    return std::max(x, vmin) + std::pow(h, 3) * k / 6.f;
  };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 vmin.vector.begin(),
                 array.vector.begin(),
                 lambda);
}

float clamp_min_smooth(float x, float vmin, float k)
{
  float h = std::max(k - std::abs(x - vmin), 0.f) / k;
  return std::max(x, vmin) + std::pow(h, 3) * k / 6.f;
}

void clamp_smooth(Array &array, float vmin, float vmax, float k)
{
  auto lambda = [&k, &vmin, &vmax](float x)
  {
    // min smooth
    float h = std::max(k - std::abs(x - vmin), 0.f) / k;
    h = std::max(x, vmin) + std::pow(h, 3) * k / 6.f;

    // max smooth
    x = h;
    h = std::max(k - std::abs(x - vmax), 0.f) / k;
    return std::min(x, vmax) - std::pow(h, 3) * k / 6.f;
  };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

Array maximum(const Array &array1, const Array &array2)
{
  Array array_out = Array(array1.shape);
  std::transform(array1.vector.begin(),
                 array1.vector.end(),
                 array2.vector.begin(),
                 array_out.vector.begin(),
                 [](float a, float b) { return std::max(a, b); });
  return array_out;
}

Array maximum(const Array &array1, const float value)
{
  Array array_out = Array(array1.shape);
  std::transform(array1.vector.begin(),
                 array1.vector.end(),
                 array_out.vector.begin(),
                 [&value](float a) { return std::max(a, value); });
  return array_out;
}

Array maximum_local(const Array &array, int ir)
{
  Array array_out = Array(array.shape);
  Array array_tmp = Array(array.shape);

  // row
  for (int i = 0; i < array.shape.x; i++)
  {
    int i1 = std::max(0, i - ir);
    int i2 = std::min(array.shape.x, i + ir + 1);

    for (int j = 0; j < array.shape.y; j++)
    {
      float max = array(i, j);
      for (int u = i1; u < i2; u++)
        if (array(u, j) > max) max = array(u, j);
      array_tmp(i, j) = max;
    }
  }

  // column
  for (int j = 0; j < array.shape.y; j++)
  {
    int j1 = std::max(0, j - ir);
    int j2 = std::min(array.shape.y, j + ir + 1);
    for (int i = 0; i < array.shape.x; i++)
    {
      float max = array_tmp(i, j);
      for (int v = j1; v < j2; v++)
        if (array_tmp(i, v) > max) max = array_tmp(i, v);
      array_out(i, j) = max;
    }
  }

  return array_out;
}

Array maximum_local_disk(const Array &array, int ir)
{
  Array array_out = array;

  int ni = array.shape.x;
  int nj = array.shape.y;

  for (int j = 0; j < nj; j++)
  {
    int q1 = std::max(0, j - ir) - j;
    int q2 = std::min(nj, j + ir + 1) - j;
    for (int i = 0; i < ni; i++)
    {
      int p1 = std::max(0, i - ir) - i;
      int p2 = std::min(ni, i + ir + 1) - i;

      for (int q = q1; q < q2; q++)
        for (int p = p1; p < p2; p++)
        {
          float r2 = (float)(p * p + q * q) / (float)ir;
          if (r2 <= 1.f)
            array_out(i, j) = std::max(array_out(i, j), array(i + p, j + q));
        }
    }
  }

  return array_out;
}

Array maximum_smooth(const Array &array1, const Array &array2, float k)
{
  if (k > 0.f)
  {
    Array array_out = Array(array1.shape);

    auto lambda = [&k](float a, float b)
    {
      float h = std::max(k - std::abs(a - b), 0.f) / k;
      return std::max(a, b) + std::pow(h, 3) * k / 6.f;
    };

    std::transform(array1.vector.begin(),
                   array1.vector.end(),
                   array2.vector.begin(),
                   array_out.vector.begin(),
                   lambda);
    return array_out;
  }
  else
    return maximum(array1, array2);
}

float maximum_smooth(const float a, const float b, float k)
{
  float h = std::max(k - std::abs(a - b), 0.f) / k;
  return std::max(a, b) + std::pow(h, 3) * k / 6.f;
}

Array minimum(const Array &array1, const Array &array2)
{
  Array array_out = Array(array1.shape);
  std::transform(array1.vector.begin(),
                 array1.vector.end(),
                 array2.vector.begin(),
                 array_out.vector.begin(),
                 [](float a, float b) { return std::min(a, b); });
  return array_out;
}

Array minimum(const Array &array1, const float value)
{
  Array array_out = Array(array1.shape);
  std::transform(array1.vector.begin(),
                 array1.vector.end(),
                 array_out.vector.begin(),
                 [&value](float a) { return std::min(a, value); });
  return array_out;
}

Array minimum_local(const Array &array, int ir)
{
  return -maximum_local(-array, ir);
}

Array minimum_local_disk(const Array &array, int ir)
{
  return -maximum_local_disk(-array, ir);
}

Array minimum_smooth(const Array &array1, const Array &array2, float k)
{
  if (k > 0.f)
  {
    Array array_out = Array(array1.shape);

    auto lambda = [&k](float a, float b)
    {
      float h = std::max(k - std::abs(a - b), 0.f) / k;
      return std::min(a, b) - std::pow(h, 3) * k / 6.f;
    };

    std::transform(array1.vector.begin(),
                   array1.vector.end(),
                   array2.vector.begin(),
                   array_out.vector.begin(),
                   lambda);
    return array_out;
  }
  else
    return minimum(array1, array2);
}

float minimum_smooth(const float a, const float b, float k)
{
  float h = std::max(k - std::abs(a - b), 0.f) / k;
  return std::min(a, b) - std::pow(h, 3) * k / 6.f;
}

void remap(Array &array, float vmin, float vmax)
{
  float min = array.min();
  float max = array.max();

  if (min != max)
  {
    auto lambda = [&min, &max, &vmin, &vmax](float x)
    { return (x - min) / (max - min) * (vmax - vmin) + vmin; };

    std::transform(array.vector.begin(),
                   array.vector.end(),
                   array.vector.begin(),
                   lambda);
  }
  else
    std::fill(array.vector.begin(), array.vector.end(), vmin);
}

void remap(Array &array, float vmin, float vmax, float from_min, float from_max)
{
  if (from_min != from_max)
  {
    auto lambda = [&from_min, &from_max, &vmin, &vmax](float x)
    { return (x - from_min) / (from_max - from_min) * (vmax - vmin) + vmin; };

    std::transform(array.vector.begin(),
                   array.vector.end(),
                   array.vector.begin(),
                   lambda);
  }
  else
    std::fill(array.vector.begin(), array.vector.end(), vmin);
}

void rescale(Array &array, float scaling, float vref)
{
  if (vref == 0.f)
    // simply multiply the values by the scaling
    array *= scaling;
  else
  {
    array -= vref;
    array *= scaling;
    array += vref;
  }
}

} // namespace hmap
