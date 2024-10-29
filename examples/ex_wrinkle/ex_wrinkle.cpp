#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto  z2 = z1;
  float wrinkle_amplitude = 0.01f;
  hmap::wrinkle(z2, wrinkle_amplitude);

  hmap::export_banner_png("ex_wrinkle.png",
                          {z1, z2},
                          hmap::Cmap::TERRAIN,
                          true);
}
