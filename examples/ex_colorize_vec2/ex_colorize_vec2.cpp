#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  hmap::Array x = hmap::slope(shape, 0.f, 1.f);
  hmap::Array y = hmap::slope(shape, 90.f, 1.f);

  hmap::Tensor col3 = hmap::colorize_vec2(x, y);
  col3.to_png("ex_colorize_vec2.png");
}
