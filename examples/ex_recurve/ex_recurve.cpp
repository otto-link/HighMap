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
  auto z0 = z;

  // curve definition (will be monotically interpolated and values
  // outside the input range avec clipped)
  const std::vector<float> t = {0.f, 0.5f, 1.f};
  const std::vector<float> v = {0.f, 0.25f, 1.f};

  hmap::recurve(z, t, v);

  hmap::export_banner_png("ex_recurve.png", {z0, z}, hmap::cmap::viridis);
}