#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  auto        z0 = z;

  hmap::sediment_deposition_particle(z, 200000, 16);

  hmap::export_banner_png("ex_sediment_deposition_particle.png",
                          {z0, z},
                          hmap::cmap::terrain,
                          true);

  z.to_file("out.bin");
}
