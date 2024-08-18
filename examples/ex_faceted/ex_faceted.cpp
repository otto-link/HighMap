#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  hmap::Array z2 = hmap::faceted(z1, hmap::neighborhood::CROSS);

  hmap::export_banner_png("ex_faceted.png", {z1, z2}, hmap::Cmap::JET, true);
}
