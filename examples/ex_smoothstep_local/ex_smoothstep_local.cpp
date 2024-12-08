#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;
  int               ir = 8;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  auto z1 = z;
  hmap::smoothstep_local(z1, ir);

  hmap::export_banner_png("ex_smoothstep_local.png",
                          {z, z1},
                          hmap::Cmap::INFERNO);
}
