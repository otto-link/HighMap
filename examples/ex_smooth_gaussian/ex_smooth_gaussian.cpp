#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;
  int               radius = 10;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::Array z0 = z;

  hmap::smooth_gaussian(z, radius);

  hmap::export_banner_png("ex_smooth_gaussian.png",
                          {z0, z},
                          hmap::Cmap::VIRIDIS);
}
