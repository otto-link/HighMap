#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  auto z1 = hmap::perlin(shape, kw, seed);
  auto z2 = hmap::perlin_billow(shape, kw, seed);
  auto z3 = hmap::perlin_mix(shape, kw, seed);

  hmap::export_banner_png("ex_perlin.png", {z1, z2, z3}, hmap::cmap::viridis);
}
