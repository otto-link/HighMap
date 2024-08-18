#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           kw = 4.f;
  float           angle = 30.f;

  hmap::Array z = hmap::gabor(shape, kw, angle);
  z.to_png("ex_gabor.png", hmap::Cmap::VIRIDIS);
}
