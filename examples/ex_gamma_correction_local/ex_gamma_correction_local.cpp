#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;
  int                      ir = 8;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::remap(z);
  hmap::gamma_correction_local(z, 0.5f, ir);
  z.to_png("ex_gamma_correction_local.png", hmap::cmap::viridis);
}
