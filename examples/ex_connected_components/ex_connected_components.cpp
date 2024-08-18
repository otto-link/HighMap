#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  uint              seed = 5;

  hmap::Array z = hmap::noise(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::clamp_min(z, 0.f);

  hmap::Array labels = hmap::connected_components(z);

  z.to_png("ex_connected_components0.png", hmap::Cmap::INFERNO);
  labels.to_png("ex_connected_components1.png", hmap::Cmap::NIPY_SPECTRAL);
}
