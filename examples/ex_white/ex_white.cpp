#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/noise.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  int                    seed = 1;

  hmap::Array z = hmap::white(shape, 0.f, 1.f, seed);
  z.to_png("ex_white.png", hmap::cmap::gray);
}
