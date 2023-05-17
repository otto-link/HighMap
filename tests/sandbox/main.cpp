#include <iostream>

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {1024, 1024};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::remap(z);

  z.to_png("out.png", hmap::cmap::terrain, true);
  z.to_file("out.bin");
}
