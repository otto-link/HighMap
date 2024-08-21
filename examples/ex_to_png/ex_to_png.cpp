#include "highmap.hpp"

int main(void)
{

  const hmap::Vec2<int>   shape = {512, 512};
  const hmap::Vec2<float> res = {4.f, 4.f};
  int                     seed = 2;

  // write png
  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                   shape,
                                   res,
                                   seed,
                                   8);
  hmap::remap(z1);
  z1.to_png("out_rgb.png", hmap::Cmap::JET);
  z1.to_png_grayscale("out_8bit.png");
  z1.to_png_grayscale("out_16bit.png", CV_16U);

  // create Array from png
  hmap::Array z2 = hmap::Array("out_8bit.png");
  hmap::Array z3 = hmap::Array("out_16bit.png");

  z1.to_raw_16bit("out.raw");

  hmap::export_banner_png("ex_to_png.png", {z1, z2, z3}, hmap::Cmap::INFERNO);
}
