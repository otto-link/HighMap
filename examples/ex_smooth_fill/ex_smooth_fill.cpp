#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;
  int                      radius = 32;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::Array z0 = z;

  hmap::smooth_fill(z, radius);

  hmap::export_banner_png("ex_smooth_fill.png",
                          {z0, z},
                          hmap::cmap::terrain,
                          true);
  z.to_file("out.bin");
}
