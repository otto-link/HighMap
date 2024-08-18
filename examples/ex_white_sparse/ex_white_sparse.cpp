#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           density = 0.1f;
  int             seed = 1;

  hmap::Array z = hmap::white_sparse(shape, 0.f, 1.f, density, seed);
  z.to_png("ex_white_sparse.png", hmap::Cmap::VIRIDIS);
}
