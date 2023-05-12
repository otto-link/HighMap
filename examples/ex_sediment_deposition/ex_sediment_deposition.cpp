#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/erosion.hpp"
#include "highmap/noise.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  auto        z0 = z;

  // talus limit defined locally
  auto talus = hmap::constant(shape, 0.2f / (float)shape[0]);
  hmap::sediment_deposition(z, talus);
  
  z0.to_png("ex_sediment_deposition0.png", hmap::cmap::gray);
  z.to_png("ex_sediment_deposition1.png", hmap::cmap::gray);
}
