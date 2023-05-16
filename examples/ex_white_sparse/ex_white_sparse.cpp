#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  float                  density = 0.1f;
  int                    seed = 1;

  hmap::Array z = hmap::white_sparse(shape, 0.f, 1.f, density, seed);
  z.to_png("ex_white_sparse.png", hmap::cmap::gray);
}
