#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z, -1.f, 1.f);

  float k = 0.1f;

  auto z1 = hmap::abs_smooth(z, k);

  auto z2 = hmap::abs_smooth(z, k, 0.5f);

  auto vshift = hmap::noise(hmap::NoiseType::PERLIN, shape, res, ++seed);
  auto z3 = hmap::abs_smooth(z, k, vshift);

  hmap::export_banner_png("ex_abs_smooth.png",
                          {z, z1, z2, z3},
                          hmap::Cmap::INFERNO);
}
