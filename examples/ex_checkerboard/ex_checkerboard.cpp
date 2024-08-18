#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 3.f};

  hmap::Array z = hmap::checkerboard(shape, kw);

  z.to_png("ex_checkerboard.png", hmap::Cmap::GRAY);
}
