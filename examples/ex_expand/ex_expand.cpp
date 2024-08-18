#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  hmap::remap(z);
  z.infos();

  int         ir = 64;
  hmap::Array z1 = z;
  hmap::expand(z1, ir);

  hmap::Array z2 = z;
  hmap::shrink(z2, ir);

  hmap::Array z3 = z;
  hmap::Array kernel = hmap::lorentzian(
      hmap::Vec2<int>(2 * ir + 1, 2 * ir + 1));
  hmap::shrink(z3, kernel);

  hmap::remap(z);
  hmap::remap(z1);
  hmap::remap(z2);
  hmap::remap(z3);

  hmap::export_banner_png("ex_expand.png",
                          {z, z1, z2, z3},
                          hmap::Cmap::TERRAIN);
}
