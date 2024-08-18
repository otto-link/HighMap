#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z0 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z0);

  hmap::Array zref = hmap::noise(hmap::NoiseType::PERLIN, shape, res, ++seed);
  hmap::remap(zref);
  hmap::gain(zref, 4.f);

  hmap::Array z1 = z0;
  hmap::match_histogram(z1, zref);

  hmap::export_banner_png("ex_match_histogram.png",
                          {zref, z0, z1},
                          hmap::Cmap::INFERNO);
}
