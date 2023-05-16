#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  auto facc = hmap::flow_accumulation_d8(z);

  z.to_png("ex_flow_accumulation_d80.png", hmap::cmap::gray);
  facc.to_png("ex_flow_accumulation_d81.png", hmap::cmap::gray);
}
