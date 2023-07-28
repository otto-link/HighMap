/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

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

Array white_density_map(const Array &density_map, uint seed)
{
  Array                                 array = Array(density_map.shape);
  std::mt19937                          gen(seed);
  std::uniform_real_distribution<float> dis(0.f, 1.f);

  for (int i = 0; i < density_map.shape[0]; i++)
    for (int j = 0; j < density_map.shape[1]; j++)
    {
      float r = dis(gen);
      if (r < density_map(i, j))
        array(i, j) = 1.f;
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
