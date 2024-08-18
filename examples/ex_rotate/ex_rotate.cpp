#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto z1 = z;
  hmap::rotate(z1, 30.f);

  hmap::export_banner_png("ex_rotate.png", {z, z1}, hmap::Cmap::VIRIDIS);
}
