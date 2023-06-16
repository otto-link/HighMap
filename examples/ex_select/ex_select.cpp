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
  hmap::remap(z);
  z.to_png("ex_select0.png", hmap::cmap::viridis);

  hmap::Array c1 = hmap::select_gt(z, 0.5f);
  c1.to_png("ex_select1.png", hmap::cmap::gray);

  hmap::Array c2 = hmap::select_lt(z, 0.5f);
  c2.to_png("ex_select2.png", hmap::cmap::gray);

  hmap::Array c3 = hmap::select_itv(z, 0.25f, 0.75f);
  c3.to_png("ex_select3.png", hmap::cmap::gray);
}
