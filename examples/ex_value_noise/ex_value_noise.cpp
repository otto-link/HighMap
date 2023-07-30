#include "highmap.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array noise_x = hmap::fbm_perlin(shape, res, seed + 1);
  hmap::Array noise_y = hmap::fbm_perlin(shape, res, seed + 2);

  hmap::Array z1 = hmap::value_noise(shape, res, seed);
  hmap::Array z2 = hmap::value_noise(shape, res, seed, &noise_x, &noise_y);

  hmap::export_banner_png("ex_value_noise.png", {z1, z2}, hmap::cmap::inferno);
}
