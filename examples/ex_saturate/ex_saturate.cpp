#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto  zs = z;
  float vmin = -0.2f;
  float vmax = 0.2f;
  float k = 0.1f;
  hmap::saturate(zs, vmin, vmax, k);

  hmap::export_banner_png("ex_saturate.png", {z, zs}, hmap::Cmap::MAGMA);
}
