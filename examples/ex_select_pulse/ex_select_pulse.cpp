#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  // select values in [0.3, 0.7] with a smooth transition
  hmap::Array c = hmap::select_pulse(z, 0.5f, 0.2f);
  hmap::remap(c);

  hmap::export_banner_png("ex_select_pulse.png", {z, c}, hmap::Cmap::INFERNO);
}
