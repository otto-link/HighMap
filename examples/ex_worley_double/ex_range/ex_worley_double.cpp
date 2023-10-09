#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::range(shape, kw, seed);

  z.to_png("ex_worley_double.png", hmap::cmap::viridis, false);
}
