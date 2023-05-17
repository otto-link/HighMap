#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;
  int                      radius = 5;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::Array zmin = hmap::minimum_local(z, radius);
  hmap::Array zmax = hmap::maximum_local(z, radius);

  z.to_png("ex_maximum_local0.png", hmap::cmap::gray);
  zmin.to_png("ex_maximum_local1.png", hmap::cmap::gray);
  zmax.to_png("ex_maximum_local2.png", hmap::cmap::gray);
}
