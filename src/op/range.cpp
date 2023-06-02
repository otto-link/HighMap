#include <algorithm>
#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

void almost_unit_identity(Array &array)
{
  array = (2.f - array) * array * array;
}

void chop(Array &array, float vmin)
{
  auto lambda = [&vmin](float x) { return x > vmin ? x : 0.f; };

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

void clamp_min(Array &array, float vmin)
{
  auto lambda = [&vmin](float x) { return x > vmin ? x : vmin; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void clamp_min_smooth(Array &array, float vmin, float k)
{
  array = maximum_smooth(array, constant(array.shape, vmin), k);
}

void clamp_max(Array &array, float vmax)
{
  auto lambda = [&vmax](float x) { return x < vmax ? x : vmax; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void clamp_max_smooth(Array &array, float vmax, float k)
{
  array = minimum_smooth(array, constant(array.shape, vmax), k);
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

Array maximum_local(const Array &array, int ir)
{
  Array array_out = Array(array.shape);
  Array array_tmp = Array(array.shape);

  // row
  for (int i = 0; i < array.shape[0]; i++)
  {
    int i1 = std::max(0, i - ir);
    int i2 = std::min(array.shape[0], i + ir + 1);

    for (int j = 0; j < array.shape[1]; j++)
    {
      float max = array(i, j);
      for (int u = i1; u < i2; u++)
        if (array(u, j) > max)
          max = array(u, j);
      array_tmp(i, j) = max;
    }
  }

  // column
  for (int j = 0; j < array.shape[1]; j++)
  {
    int j1 = std::max(0, j - ir);
    int j2 = std::min(array.shape[1], j + ir + 1);
    for (int i = 0; i < array.shape[0]; i++)
    {
      float max = array_tmp(i, j);
      for (int v = j1; v < j2; v++)
        if (array_tmp(i, v) > max)
          max = array_tmp(i, v);
      array_out(i, j) = max;
    }
  }

  return array_out;
}

Array maximum_local_disk(const Array &array, int ir)
{
  Array array_out = array;

  int ni = array.shape[0];
  int nj = array.shape[1];

  for (int i = 0; i < ni; i++)
  {
    int p1 = std::max(0, i - ir) - i;
    int p2 = std::min(ni, i + ir) - i;
    for (int j = 0; j < nj; j++)
    {
      int q1 = std::max(0, j - ir) - j;
      int q2 = std::min(nj, j + ir) - j;
      for (int p = p1; p < p2; p++)
        for (int q = q1; q < q2; q++)
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

Array mean_local(const Array &array, int ir)
{
  Array array_out = Array(array.shape);
  Array array_tmp = Array(array.shape);

  // row
  for (int i = 0; i < array.shape[0]; i++)
  {
    int i1 = std::max(0, i - ir);
    int i2 = std::min(array.shape[0], i + ir + 1);

    for (int j = 0; j < array.shape[1]; j++)
    {
      float sum = 0.f;
      for (int u = i1; u < i2; u++)
        sum += array(u, j);
      array_tmp(i, j) = sum / (float)(i2 - i1);
    }
  }

  // column
  for (int j = 0; j < array.shape[1]; j++)
  {
    int j1 = std::max(0, j - ir);
    int j2 = std::min(array.shape[1], j + ir + 1);
    for (int i = 0; i < array.shape[0]; i++)
    {
      float sum = 0.f;
      for (int v = j1; v < j2; v++)
        sum += array_tmp(i, v);
      array_out(i, j) = sum / (float)(j2 - j1);
    }
  }

  return array_out;
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

void remap(Array &array, float vmin, float vmax)
{
  float min = array.min();
  float max = array.max();

  auto lambda = [&min, &max, &vmin, &vmax](float x)
  { return (x - min) / (max - min) * (vmax - vmin) + vmin; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

void remap(Array &array, float vmin, float vmax, float from_min, float from_max)
{
  auto lambda = [&from_min, &from_max, &vmin, &vmax](float x)
  { return (x - from_min) / (from_max - from_min) * (vmax - vmin) + vmin; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

} // namespace hmap
