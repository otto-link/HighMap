#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::remap(z);
  auto z0 = z;
  hmap::zeroed_edges(z);

  hmap::export_banner_png("ex_zeroed_edges.png", {z0, z}, hmap::cmap::inferno);
}
