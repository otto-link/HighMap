#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto facc = hmap::flow_accumulation_d8(z);

  z.to_png("ex_flow_accumulation_d80.png", hmap::Cmap::TERRAIN, true);
  facc.to_png("ex_flow_accumulation_d81.png", hmap::Cmap::HOT);
}
