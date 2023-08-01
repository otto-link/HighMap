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

  int         ir = 32;
  hmap::Array c = hmap::select_blob_log(z, ir);

  z.to_png("ex_select_blob0.png", hmap::cmap::viridis);
  c.to_png("ex_select_blob1.png", hmap::cmap::viridis);
}
