#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  hmap::remap(z);
  z.infos();

  int  ir = 32;
  auto z1 = z;
  hmap::expand(z1, ir);

  auto z2 = z;
  hmap::shrink(z2, ir);

  hmap::remap(z);
  hmap::remap(z1);
  hmap::remap(z2);

  hmap::export_banner_png("ex_expand.png", {z, z1, z2}, hmap::cmap::viridis);
}
