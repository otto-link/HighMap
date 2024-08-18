#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);
  auto z0 = z;

  hmap::hydraulic_benes(z, 50);

  hmap::export_banner_png("ex_hydraulic_benes.png",
                          {z0, z},
                          hmap::Cmap::TERRAIN,
                          true);
}
