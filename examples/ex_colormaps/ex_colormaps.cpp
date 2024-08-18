#include <iostream>

#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {128, 128};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  std::cout << "Available colormaps:" << std::endl;
  for (auto &[cmap, cmap_name] : hmap::cmap_as_string)
    std::cout << " - " << cmap << ": " << cmap_name << std::endl;

  z.to_png("ex_colormaps1.png", hmap::Cmap::BONE);
  z.to_png("ex_colormaps2.png", hmap::Cmap::GRAY);
  z.to_png("ex_colormaps3.png", hmap::Cmap::HOT);
  z.to_png("ex_colormaps4.png", hmap::Cmap::INFERNO);
  z.to_png("ex_colormaps5.png", hmap::Cmap::JET);
  z.to_png("ex_colormaps6.png", hmap::Cmap::MAGMA);
  z.to_png("ex_colormaps7.png", hmap::Cmap::NIPY_SPECTRAL);
  z.to_png("ex_colormaps8.png", hmap::Cmap::SEISMIC);
  z.to_png("ex_colormaps9.png", hmap::Cmap::TERRAIN, true);
  z.to_png("ex_colormaps10.png", hmap::Cmap::VIRIDIS);
}
