#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  float       overlap = 0.5f;

  hmap::Vec2<int> tiling = {4, 3};

  auto zp = hmap::make_periodic_tiling(z, overlap, tiling);

  hmap::export_banner_png("ex_make_periodic_tiling.png",
                          {z, zp},
                          hmap::Cmap::TERRAIN);
}
