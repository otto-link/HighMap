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

  auto  z_peak = z;
  int   ir = 16;
  float gamma = 2.f;
  hmap::recast_peak(z_peak, ir, gamma);

  hmap::export_banner_png("ex_recast.png",
                          {z, z_peak},
                          hmap::cmap::terrain,
                          true);
}
