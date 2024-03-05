#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z1 = hmap::fbm_worley(shape, res, seed);
  hmap::Array z2 = hmap::fbm_worley_double(shape, res, seed);

  z1.to_png("ex_fbm_worley.png", hmap::cmap::terrain, true);
  z2.to_png("ex_fbm_worley_double.png", hmap::cmap::terrain, true);
}
