#include <iostream>

#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  z.to_png("ex_fbm_perlin.png", hmap::cmap::gray);
}
