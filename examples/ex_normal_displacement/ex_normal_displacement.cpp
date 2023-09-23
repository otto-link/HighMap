#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  shape = {512, 512};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z1 = hmap::fbm_perlin(shape, res, seed);

  hmap::Array z2 = z1;
  hmap::normal_displacement(z2, 5.f, 4, false);
  z2.infos();
  hmap::export_banner_png("ex_normal_displacement.png",
                          {z1, z2},
                          hmap::cmap::terrain);
}
