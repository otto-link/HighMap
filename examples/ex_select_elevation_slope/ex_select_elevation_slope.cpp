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
  hmap::remap(z, -1.f, 2.f);

  hmap::Array c = hmap::select_elevation_slope(z);

  z.to_png("ex_select_elevation_slope0.png", hmap::cmap::jet);
  c.to_png("ex_select_elevation_slope1.png", hmap::cmap::jet);
}
