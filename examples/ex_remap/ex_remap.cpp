#include <iostream>

#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  std::cout << z.min() << " " << z.max() << std::endl;

  hmap::remap(z, 0.f, 1.f);
  std::cout << z.min() << " " << z.max() << std::endl;
}
