#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  auto        z1 = z;
  auto        z2 = z;

  float angle = 30.f;
  hmap::steepen_convective(z1, angle, 1, 0.5f);

  hmap::export_banner_png("ex_steepen_convective.png",
                          {z, z1},
                          hmap::cmap::terrain);
}
