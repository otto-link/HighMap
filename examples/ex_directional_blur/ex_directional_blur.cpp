#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  auto  z1 = z;
  int   ir = 64;
  float angle = 30.f;
  float intensity = 1.f;

  hmap::directional_blur(z1, ir, angle, intensity);
  hmap::remap(z1);

  hmap::export_banner_png("ex_directional_blur.png",
                          {z, z1},
                          hmap::Cmap::TERRAIN,
                          true);
}
