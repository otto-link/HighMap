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

  int         ir = 8;
  hmap::Array w = hmap::valley_width(z, ir);

  z.to_png("ex_valley_width0.png", hmap::cmap::terrain);
  w.to_png("ex_valley_width1.png", hmap::cmap::inferno);
}
