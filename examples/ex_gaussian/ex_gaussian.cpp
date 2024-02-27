#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::remap(z, -1.f, 1.f);

  float       sigma = 0.5f;
  hmap::Array zg = hmap::gaussian(z, sigma);

  hmap::export_banner_png("ex_gaussian.png", {z, zg}, hmap::cmap::viridis);
}
