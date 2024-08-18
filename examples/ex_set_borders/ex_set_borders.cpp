#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;
  float             value = 0.f;
  int               nbuffer = 100;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  hmap::set_borders(z, value, nbuffer);
  z.to_png("ex_set_borders.png", hmap::Cmap::TERRAIN, true);
}
