#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;
  int               radius = 10;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::Array zm = hmap::mean_local(z, radius);

  z.to_png("ex_mean_local0.png", hmap::Cmap::VIRIDIS);
  zm.to_png("ex_mean_local1.png", hmap::Cmap::VIRIDIS);
}
