#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto z1 = z;
  hmap::flip_lr(z1);

  auto z2 = z;
  hmap::flip_ud(z2);

  hmap::export_banner_png("ex_flip_ud.png", {z, z1, z2}, hmap::Cmap::VIRIDIS);
}
