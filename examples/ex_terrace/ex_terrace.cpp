#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed++);

  int   nlevels = 5;
  float gain = 0.9f;
  float noise_ratio = 0.5f;

  auto zp = z;
  hmap::terrace(zp, seed, nlevels, &z, gain, noise_ratio);

  hmap::export_banner_png("ex_terrace.png", {z, zp}, hmap::Cmap::TERRAIN, true);
}
