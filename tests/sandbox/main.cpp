#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{

  const std::vector<int>   shape = {512, 512};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 2;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed, 8);

  z.to_png("out.png", hmap::cmap::gray);
}
