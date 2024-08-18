#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  auto        z0 = z;

  hmap::thermal_flatten(z, 8.f / shape.x, 10);

  hmap::export_banner_png("ex_thermal_flatten.png",
                          {z0, z},
                          hmap::Cmap::TERRAIN,
                          true);
}
