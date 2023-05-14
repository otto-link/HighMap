#include <algorithm>
#include <cmath>

#include "highmap/array.hpp"

namespace hmap
{

//----------------------------------------------------------------------
// Main operator(s)
//----------------------------------------------------------------------

void almost_unit_identity(Array &array)
{
  array = (2.f - array) * array * array;
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

void clamp_max(Array &array, float vmax)
{
  auto lambda = [&vmax](float x) { return x < vmax ? x : vmax; };

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array.vector.begin(),
                 lambda);
}

Array maximum(Array &array1, Array &array2)
{
  Array array_out = Array(array1.shape);
  std::transform(array1.vector.begin(),
                 array1.vector.end(),
                 array2.vector.begin(),
                 array_out.vector.begin(),
                 [](float a, float b) { return std::max(a, b); });
  return array_out;
}

Array maximum_local1d_i(Array &array, int ir)
{
  Array array_out = Array(array.shape);

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
      array_out(i, j) = max;
    }
  }
  return array_out;
}

Array maximum_local1d_j(Array &array, int ir)
{
  Array array_out = Array(array.shape);

  for (int j = 0; j < array.shape[1]; j++)
  {
    int j1 = std::max(0, j - ir);
    int j2 = std::min(array.shape[1], j + ir + 1);
    for (int i = 0; i < array.shape[0]; i++)
    {
      float max = array(i, j);
      for (int v = j1; v < j2; v++)
        if (array(i, v) > max)
          max = array(i, v);
      array_out(i, j) = max;
    }
  }
  return array_out;
}

Array maximum_local(Array &array, int ir)
{
  Array array_out = Array(array.shape);
  array_out = maximum_local1d_i(array, ir);
  array_out = maximum_local1d_j(array_out, ir);
  return array_out;
}

Array maximum_smooth(Array &array1, Array &array2, float k)
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

Array minimum(Array &array1, Array &array2)
{
  Array array_out = Array(array1.shape);
  std::transform(array1.vector.begin(),
                 array1.vector.end(),
                 array2.vector.begin(),
                 array_out.vector.begin(),
                 [](float a, float b) { return std::min(a, b); });
  return array_out;
}

Array minimum_local1d_i(Array &array, int ir)
{
  Array array_out = Array(array.shape);

  for (int i = 0; i < array.shape[0]; i++)
  {
    int i1 = std::max(0, i - ir);
    int i2 = std::min(array.shape[0], i + ir + 1);

    for (int j = 0; j < array.shape[1]; j++)
    {
      float min = 1e9;
      for (int u = i1; u < i2; u++)
        if (array(u, j) < min)
          min = array(u, j);
      array_out(i, j) = min;
    }
  }
  return array_out;
}

Array minimum_local1d_j(Array &array, int ir)
{
  Array array_out = Array(array.shape);

  for (int j = 0; j < array.shape[1]; j++)
  {
    int j1 = std::max(0, j - ir);
    int j2 = std::min(array.shape[1], j + ir + 1);
    for (int i = 0; i < array.shape[0]; i++)
    {
      float min = 1e9;
      for (int v = j1; v < j2; v++)
        if (array(i, v) < min)
          min = array(i, v);
      array_out(i, j) = min;
    }
  }
  return array_out;
}

Array minimum_local(Array &array, int ir)
{
  Array array_out = Array(array.shape);
  array_out = minimum_local1d_i(array, ir);
  array_out = minimum_local1d_j(array_out, ir);
  return array_out;
}

Array minimum_smooth(Array &array1, Array &array2, float k)
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
