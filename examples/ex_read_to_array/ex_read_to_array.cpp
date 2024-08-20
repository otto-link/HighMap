#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 128};
  hmap::Vec2<float> res = {4.f, 2.f};
  int               seed = 1;

  hmap::Array z0 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z0, 0.f, 1.f);

  z0.to_png("out1.png", hmap::Cmap::GRAY); // grayscale
  z0.to_png("out2.png", hmap::Cmap::JET);  // RGB image

  hmap::Array z1 = hmap::read_to_array("out1.png");
  hmap::Array z2 = hmap::read_to_array("out2.png"); // converted to grayscale

  hmap::Array z3 = hmap::Array("out1.png");

  hmap::export_banner_png("ex_read_to_array.png",
                          {z0, z1, z2, z3},
                          hmap::Cmap::INFERNO);
}
