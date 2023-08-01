#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z1 = hmap::fbm_perlin(shape, res, seed);
  hmap::Array z2 = hmap::hybrid_fbm_perlin(shape, res, seed);

  hmap::remap(z1);
  hmap::remap(z2);

  hmap::export_banner_png("ex_hybrid_fbm_perlin.png",
                          {z1, z2},
                          hmap::cmap::terrain,
                          true);
}
