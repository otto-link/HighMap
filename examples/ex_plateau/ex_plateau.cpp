#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto zp = z;
  hmap::plateau(zp, 64, 4.f);

  hmap::export_banner_png("ex_plateau.png", {z, zp}, hmap::Cmap::TERRAIN);
}
