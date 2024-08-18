#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  hmap::clamp_min(z, 0.f);
  auto d = hmap::distance_transform(z);

  z.to_png("ex_distance_transform0.png", hmap::Cmap::VIRIDIS);
  d.to_png("ex_distance_transform1.png", hmap::Cmap::VIRIDIS);
}
