#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 2;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  int         ir = 32;
  hmap::Array w = hmap::select_valley(z, ir, true);

  hmap::export_banner_png("ex_select_valley.png", {z, w}, hmap::Cmap::INFERNO);
}
