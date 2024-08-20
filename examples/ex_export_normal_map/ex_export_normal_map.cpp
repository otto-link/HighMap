#include "highmap.hpp"

int main(void)
{

  const hmap::Vec2<int>   shape = {512, 512};
  const hmap::Vec2<float> res = {1.f, 4.f};
  int                     seed = 0;

  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z1);

  // create Array from png
  hmap::export_normal_map_png_8bit("ex_export_normal_map0.png", z1);
  hmap::export_normal_map_png_16bit("ex_export_normal_map1.png", z1);
}
