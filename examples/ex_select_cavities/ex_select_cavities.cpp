#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  int         ir = 32;
  hmap::Array c1 = hmap::select_cavities(z, ir);        // holes
  hmap::Array c2 = hmap::select_cavities(z, ir, false); // bumps

  hmap::remap(c1);
  hmap::remap(c2);

  hmap::export_banner_png("ex_select_cavities.png",
                          {z, c1, c2},
                          hmap::Cmap::INFERNO);
}
