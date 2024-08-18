#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  z += 0.1f * hmap::white(shape, 0.f, 1.f, seed);
  auto z0 = z;

  hmap::median_3x3(z);

  hmap::export_banner_png("ex_median_3x3.png", {z0, z}, hmap::Cmap::VIRIDIS);
}
