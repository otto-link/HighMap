#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;
  int               radius = 20;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::Array z0 = z;

  hmap::smooth_cpulse(z, radius);

  hmap::export_banner_png("ex_smooth_cpulse.png", {z0, z}, hmap::cmap::viridis);
}
