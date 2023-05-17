#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::Array c = hmap::curvature_gaussian(z);

  z.to_png("ex_curvature_gaussian0.png", hmap::cmap::gray);
  c.to_png("ex_curvature_gaussian1.png", hmap::cmap::gray);
}