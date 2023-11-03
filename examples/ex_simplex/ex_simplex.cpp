#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  auto z1 = hmap::simplex(shape, kw, seed);
  auto z2 = hmap::fbm_simplex(shape, kw, seed);

  hmap::export_banner_png("ex_simplex.png", {z1, z2}, hmap::cmap::viridis);
}
