#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::Array z0 = z;

  hmap::Array dx = hmap::fbm_perlin(shape, {4.f, 4.f}, seed + 1);
  hmap::Array dy = hmap::fbm_perlin(shape, {4.f, 4.f}, seed + 2);

  // adjust warping maximum displacement to +- 64 pixels in both
  // directions
  float scale = 32.f;
  remap(dx, -scale, scale);
  remap(dy, -scale, scale);

  hmap::warp(z, &dx, &dy);

  hmap::export_banner_png("ex_warp.png", {z0, z}, hmap::cmap::viridis);

  z.to_file("out.bin");
}
