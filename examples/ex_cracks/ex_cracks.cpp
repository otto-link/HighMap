#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  shape = {1024, 1024};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::Array z2 = z1;

  hmap::recast_cracks(z1);

  float cut_min = 0.1; // drives width
  float cut_max = 0.9; // drives density

  hmap::recast_cracks(z2, cut_min, cut_max);

  hmap::export_banner_png("ex_cracks.png", {z1, z2}, hmap::Cmap::TERRAIN, true);
}
