#include "highmap.hpp"

int main(void)
{

  const hmap::Vec2<int>   shape = {512, 512};
  const hmap::Vec2<float> res = {4.f, 4.f};
  int                     seed = 2;

  // write png
  hmap::Array z1 = hmap::fbm_perlin(shape, res, seed, 8);
  hmap::remap(z1);
  z1.to_png("out.png", hmap::cmap::gray);

  // create Array from png
  hmap::Array z2 = hmap::Array("out.png");
  z2.infos();

  z2.to_png16bit("out_16bit.png");

  hmap::export_banner_png("ex_to_png.png", {z1, z2}, hmap::cmap::inferno);
}
