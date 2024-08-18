#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);
  z.to_png("ex_select0.png", hmap::Cmap::VIRIDIS);

  hmap::Array c1 = hmap::select_gt(z, 0.5f);
  c1.to_png("ex_select1.png", hmap::Cmap::GRAY);

  hmap::Array c2 = hmap::select_lt(z, 0.5f);
  c2.to_png("ex_select2.png", hmap::Cmap::GRAY);

  hmap::Array c3 = hmap::select_interval(z, 0.25f, 0.75f);
  c3.to_png("ex_select3.png", hmap::Cmap::GRAY);

  hmap::Array c4 = hmap::select_eq(c3, 1.f);
  c4.to_png("ex_select4.png", hmap::Cmap::GRAY);
}
