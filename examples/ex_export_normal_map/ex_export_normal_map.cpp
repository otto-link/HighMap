#include "highmap.hpp"

int main(void)
{

  const hmap::Vec2<int>   shape = {512, 512};
  const hmap::Vec2<float> res = {1.f, 4.f};
  int                     seed = 2;

  // write png
  hmap::Array z1 = hmap::perlin(shape, res, seed);
  hmap::remap(z1);
  z1.to_png_grayscale_8bit("out_8bit.png");
  z1.to_png_grayscale_16bit("out_16bit.png");

  z1.to_raw_16bit("out.raw");

  // create Array from png
  hmap::export_normal_map_png_8bit("out_nmap_8bit.png", z1);
  hmap::export_normal_map_png_16bit("out_nmap_16bit.png", z1);
}
