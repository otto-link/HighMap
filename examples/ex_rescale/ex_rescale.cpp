#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z1);

  auto z2 = z1;
  hmap::rescale(z2, 0.5f);

  auto  z3 = z1;
  float vref = z1.mean();
  hmap::rescale(z3, 0.5f, vref);

  hmap::export_banner_png("ex_rescale.png", {z1, z2, z3}, hmap::Cmap::INFERNO);
}
