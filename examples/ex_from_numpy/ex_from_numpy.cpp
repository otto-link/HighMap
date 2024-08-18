#include <iostream>

#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {512, 256};
  hmap::Vec2<float> res = {4.f, 2.f};
  int               seed = 1;

  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  z1.to_numpy("out.npy");

  hmap::Array z2;
  z2.from_numpy("out.npy");

  hmap::export_banner_png("ex_from_numpy.png", {z1, z2}, hmap::Cmap::INFERNO);
}
