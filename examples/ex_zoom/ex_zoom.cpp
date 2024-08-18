#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  float             zoom = 2.f;
  hmap::Vec2<float> center = {0.25f, 0.f};

  auto z1 = hmap::zoom(z, zoom);
  auto z2 = hmap::zoom(z, zoom, false, center); // false for not periodic

  // to avoid: zoom < 1.f, w/ periodic boundaries
  auto z3 = hmap::zoom(z, 0.5f, true);

  hmap::export_banner_png("ex_zoom.png", {z, z1, z2, z3}, hmap::Cmap::MAGMA);
}
