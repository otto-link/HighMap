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

  auto z1 = z;
  hmap::clamp(z1, -0.1f, 0.1f);

  auto z2 = z;
  hmap::clamp_max_smooth(z2, 0.1f);

  auto z3 = z;
  hmap::clamp_min_smooth(z3, -0.1f);

  hmap::export_banner_png("ex_clamp.png", {z1, z2, z3}, hmap::cmap::viridis);
}
