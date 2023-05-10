#include <random>

#include "highmap/array.hpp"

namespace hmap
{

Array white(std::vector<int> shape, float a, float b, uint seed)
{
  Array                                 array = Array(shape);
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(a, b);
  for (auto &v : array.vector)
  {
    v = dis(gen);
  }
  return array;
}

Array white_sparse(std::vector<int> shape,
                   float            a,
                   float            b,
                   float            density,
                   uint             seed)
{
  Array                                 array = Array(shape);
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(a, b);

  for (auto &v : array.vector)
  {
    float r = dis(gen);
    float d = (r - a) / (b - a); // remap to [0, 1]
    if (d < density)
      v = r;
  }
  return array;
}

} // namespace hmap
