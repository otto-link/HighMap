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

void clamp_max_smooth(Array &array, float vmin, float k)
{
  array = minimum_smooth(array, constant(array.shape, vmin), k);
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
