#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/io.hpp"
#include "highmap/math.hpp"
#include "highmap/noise.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  std::vector<float> k(32);
  std::fill(k.begin(), k.end(), 1.f / 32.f);

  auto zc = hmap::convolve1d_i(z, k);
  zc = hmap::convolve1d_j(zc, k);

  hmap::export_banner_png("ex_convolve1d_ij.png", {z, zc}, hmap::cmap::gray);
}
