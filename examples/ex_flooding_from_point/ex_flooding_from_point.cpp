#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  int i, j;

  i = 32;
  j = 64;
  hmap::Array fmap1 = hmap::flooding_from_point(z, i, j);

  i = 64;
  j = 150;
  hmap::Array fmap2 = hmap::flooding_from_point(z, i, j);

  hmap::Array fmap3 = hmap::flooding_from_point(z, {32, 64}, {64, 150});

  hmap::export_banner_png(
      "ex_flooding_from_point.png",
      {z, fmap1, z + fmap1, fmap2, z + fmap2, fmap3, z + fmap3},
      hmap::Cmap::JET);
}
