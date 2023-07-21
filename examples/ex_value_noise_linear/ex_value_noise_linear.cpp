#include "highmap.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array noise = 0.2f * hmap::fbm_perlin(shape, res, seed);

  hmap::Array z1 = hmap::value_noise_linear(shape, res, seed);
  hmap::Array z2 = hmap::value_noise_linear(shape, res, seed);
  hmap::warp_fbm(z2, 32.f, res, seed);

  hmap::export_banner_png("ex_value_noise_linear.png",
                          {z1, z2},
                          hmap::cmap::inferno);
}
