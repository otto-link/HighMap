#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           slant_ratio = 0.5f;
  float           angle = 30.f;

  hmap::Array z = hmap::cubic_pulse_truncated(shape, slant_ratio, angle);
  z.to_png("ex_cubic_pulse_truncated.png", hmap::Cmap::VIRIDIS);
}
