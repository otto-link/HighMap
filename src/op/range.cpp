#include <algorithm>

#include "highmap/array.hpp"

namespace hmap
{

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