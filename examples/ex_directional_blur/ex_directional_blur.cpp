#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  int   ir = 64;
  float angle = 30.f;
  float intensity = 1.f;

  auto z1 = z;
  {
    hmap::directional_blur(z1, ir, angle, intensity);
    hmap::remap(z1);
  }

  auto z2 = z;
  {
    float stretch = 3.f;
    hmap::directional_blur(z2, ir, angle, intensity, stretch);
    hmap::remap(z2);
  }

  auto z3 = z;
  {
    float stretch = 1.f;
    float spread = 0.f;
    hmap::directional_blur(z3, ir, angle, intensity, stretch, spread);
    hmap::remap(z3);
  }

  hmap::export_banner_png("ex_directional_blur.png",
                          {z, z1, z2, z3},
                          hmap::Cmap::JET);
}
