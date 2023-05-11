#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/noise.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::perlin(shape, res, seed);

  auto zr = z.resample_to_shape({32, 32});

  zr.to_png("ex_resample_to_shape.png", hmap::cmap::gray);
}
