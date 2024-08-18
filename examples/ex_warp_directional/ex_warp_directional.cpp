#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  shape = {512, 512};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  hmap::Array mask = z1;
  hmap::remap(mask);

  hmap::Array z2 = z1;
  hmap::warp_directional(z2, 30.f, 0.02f, 0, true);

  hmap::export_banner_png("ex_warp_directional.png",
                          {z1, z2},
                          hmap::Cmap::TERRAIN);
}
