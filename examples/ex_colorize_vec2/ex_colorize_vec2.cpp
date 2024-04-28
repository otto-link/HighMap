#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  hmap::Array x = hmap::slope(shape, 0.f, 1.f);
  hmap::Array y = hmap::slope(shape, 90.f, 1.f);

  std::vector<uint8_t> img = hmap::colorize_vec2(x, y);

  hmap::write_png_rgb_8bit("ex_colorize_vec2.png", img, shape);
}
