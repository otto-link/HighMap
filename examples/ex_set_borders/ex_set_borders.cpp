#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/noise.hpp"
#include "highmap/op.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;
  float                    value = 0.f;
  int                      nbuffer = 100;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  hmap::set_borders(z, value, nbuffer);
  z.to_png("ex_set_borders.png", hmap::cmap::gray);
}
