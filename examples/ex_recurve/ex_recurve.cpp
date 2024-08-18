#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);
  auto z0 = z;

  // curve definition (will be monotically interpolated and values
  // outside the input range avec clipped)
  std::vector<float> t = {0.f, 0.5f, 1.f};
  std::vector<float> v = {0.f, 0.25f, 1.f};

  hmap::recurve(z, t, v);

  hmap::export_banner_png("ex_recurve.png", {z0, z}, hmap::Cmap::VIRIDIS);
}
