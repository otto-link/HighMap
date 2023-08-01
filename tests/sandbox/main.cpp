#include "highmap.hpp"

int main(void)
{

  const hmap::Vec2<int>   shape = {512, 512};
  const hmap::Vec2<float> res = {4.f, 4.f};
  int                     seed = 2;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed, 8);
  z.to_png("out.png", hmap::cmap::gray);
}
