#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/math.hpp"
#include "highmap/noise.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  auto        z0 = z;

  // stratification features (nb of layers, elevations and gamma
  // correction factors)
  int  ns = 10;
  auto hs = hmap::linspace_jitted(z.min(), z.max(), ns, 0.9f, seed);
  auto gs = hmap::random_vector(0.2, 0.8, ns - 1, seed);

  hmap::stratify(z, hs, gs);

  hmap::export_banner_png("ex_stratify.png", {z0, z}, hmap::cmap::gray);
}
