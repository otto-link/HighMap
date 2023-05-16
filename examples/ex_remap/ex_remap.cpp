#include <iostream>

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  std::cout << z.min() << " " << z.max() << std::endl;

  hmap::remap(z, 0.f, 1.f);
  std::cout << z.min() << " " << z.max() << std::endl;
}
