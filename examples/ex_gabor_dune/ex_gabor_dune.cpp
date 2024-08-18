#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           kw = 8.f;
  float           angle = 30.f;
  float           xtop = 0.7f;
  float           xbottom = 1.f;

  hmap::Array z = hmap::gabor_dune(shape, kw, angle, xtop, xbottom);
  z.to_png("ex_gabor_dune.png", hmap::Cmap::VIRIDIS);
}
