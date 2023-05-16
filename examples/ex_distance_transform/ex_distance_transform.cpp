#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  hmap::clamp_min(z, 0.f);
  auto d = hmap::distance_transform(z);

  z.to_png("ex_distance_transform0.png", hmap::cmap::gray);
  d.to_png("ex_distance_transform1.png", hmap::cmap::gray);
}
