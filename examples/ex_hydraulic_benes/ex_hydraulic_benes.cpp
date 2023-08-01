#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  auto        z0 = z;

  hmap::hydraulic_benes(z, 50);

  hmap::export_banner_png("ex_hydraulic_benes.png",
                          {z0, z},
                          hmap::cmap::terrain,
                          true);
}
