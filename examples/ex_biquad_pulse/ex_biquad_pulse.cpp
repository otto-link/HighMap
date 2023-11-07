#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  
  hmap::Array z = hmap::biquad_pulse(shape);

  z.to_png("ex_biquad_pulse.png", hmap::cmap::inferno);
}
