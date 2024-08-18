#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  // contrast / brightness
  auto z1 = hmap::scan_mask(z, 0.8f, 0.5f);
  auto z2 = hmap::scan_mask(z, 0.5f, 0.2f);

  hmap::export_banner_png("ex_scan_mask.png", {z, z1, z2}, hmap::Cmap::MAGMA);
}
