#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           kw = 4.f;
  float           angle = 30.f;
  int             width = 32;
  float           density = 0.05f;
  uint            seed = 1;

  hmap::Array z = hmap::gabor_noise(shape, kw, angle, width, density, seed);
  z.to_png("ex_gabor_noise.png", hmap::Cmap::VIRIDIS);
}
