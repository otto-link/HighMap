#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto zt = hmap::translate(z, 0.25f, -0.1f, false);

  hmap::export_banner_png("ex_translate.png", {z, zt}, hmap::Cmap::JET);
}
