#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/noise.hpp"
#include "highmap/op.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::set_borders(z, z.min(), 100);

  hmap::Array talus = hmap::gradient_talus(z);

  auto facc = hmap::flow_accumulation_dinf(z, talus.max());

  // very high values are less relevant
  hmap::clamp_max(facc, 100.f);

  z.to_png("ex_flow_accumulation_dinf0.png", hmap::cmap::gray);
  facc.to_png("ex_flow_accumulation_dinf1.png", hmap::cmap::gray);
}
