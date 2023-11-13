#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  z.to_png("ex_colormaps1.png", hmap::cmap::bone);
  z.to_png("ex_colormaps2.png", hmap::cmap::gray);
  z.to_png("ex_colormaps3.png", hmap::cmap::hot);
  z.to_png("ex_colormaps4.png", hmap::cmap::inferno);
  z.to_png("ex_colormaps5.png", hmap::cmap::jet);
  z.to_png("ex_colormaps6.png", hmap::cmap::magma);
  z.to_png("ex_colormaps7.png", hmap::cmap::nipy_spectral);
  z.to_png("ex_colormaps8.png", hmap::cmap::seismic);
  z.to_png("ex_colormaps9.png", hmap::cmap::terrain, true);
  z.to_png("ex_colormaps10.png", hmap::cmap::viridis);
}
