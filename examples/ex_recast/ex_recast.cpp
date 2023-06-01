#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::remap(z);

  auto z1 = z;
  {
    int   ir = 16;
    float gamma = 2.f;
    hmap::recast_peak(z1, ir, gamma);
  }

  auto z2 = z;
  {
    float zcut = 0.5f;
    float gamma = 4.f;
    hmap::recast_canyon(z2, zcut, gamma);
  }

  z2.to_file("out.bin");

  hmap::export_banner_png("ex_recast.png",
                          {z, z1, z2},
                          hmap::cmap::terrain,
                          true);
}
