#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  hmap::Array z = hmap::bump(shape);

  z.to_png("ex_bump.png", hmap::cmap::inferno);
}
