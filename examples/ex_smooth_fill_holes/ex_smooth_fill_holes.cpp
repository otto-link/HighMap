#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;
  int                      radius = 2;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  auto z1 = z;
  hmap::smooth_fill_holes(z1, radius);

  auto z2 = z;
  hmap::smooth_fill_smear_peaks(z2, radius);

  hmap::export_banner_png("ex_smooth_fill_holes.png",
                          {z, z1, z2},
                          hmap::cmap::terrain,
                          true);
  z.to_file("out.bin");
}
