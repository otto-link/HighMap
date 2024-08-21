#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           sigma = 0.2f;

  hmap::Array z = hmap::gaussian_pulse(shape, sigma);

  hmap::Tensor col3 = hmap::colorize_histogram(z);
  col3.to_png("ex_colorize_histrogram.png");
}
