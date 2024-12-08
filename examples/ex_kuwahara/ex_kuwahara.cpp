#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto z1 = z;
  int  ir = 8;
  hmap::kuwahara(z1, ir);

  auto  z2 = z;
  float mix = 0.25f;
  hmap::kuwahara(z2, ir, mix);

  hmap::export_banner_png("ex_kuwahara.png", {z, z1, z2}, hmap::Cmap::INFERNO);
}
