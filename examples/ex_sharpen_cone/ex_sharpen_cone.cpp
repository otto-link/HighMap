#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z0 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  auto        z1 = z0;

  int ir = 16;
  hmap::sharpen_cone(z1, ir);

  hmap::export_banner_png("ex_sharpen_clone.png", {z0, z1}, hmap::Cmap::JET);
}
