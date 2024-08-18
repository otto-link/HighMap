#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  float talus = 1.f / shape.x;

  hmap::Array z1 = z;
  hmap::fill_talus(z1, talus, seed);

  // same algo on a coarser mesh to spare some computational time
  hmap::Array z2 = z;
  hmap::fill_talus_fast(z2, hmap::Vec2<int>(64, 64), talus, seed);

  hmap::export_banner_png("ex_fill_talus.png",
                          {z, z1, z2},
                          hmap::Cmap::INFERNO);
}
