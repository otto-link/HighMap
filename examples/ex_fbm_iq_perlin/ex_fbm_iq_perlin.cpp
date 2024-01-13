#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  shape = {512, 512};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z1 = hmap::fbm_perlin(shape, res, seed);
  hmap::Array z2 = hmap::fbm_iq_perlin(shape, res, seed, 0.0f, 5.f);

  hmap::remap(z1);
  hmap::remap(z2);

  hmap::export_banner_png("ex_fbm_iq_perlin.png",
                          {z1, z2},
                          hmap::cmap::terrain,
                          true);
}
