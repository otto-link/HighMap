#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  int   iterations = 5;
  float k_smooth;

  auto z1 = z;
  k_smooth = 0.f;
  hmap::fold(z1, iterations, k_smooth);
  hmap::remap(z1);

  auto z2 = z;
  k_smooth = 0.05f;
  hmap::fold(z2, iterations, k_smooth);
  hmap::remap(z2);

  hmap::export_banner_png("ex_fold.png", {z, z1, z2}, hmap::Cmap::INFERNO);
}
