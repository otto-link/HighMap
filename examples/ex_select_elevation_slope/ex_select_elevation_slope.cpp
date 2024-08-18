#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z, -1.f, 2.f);

  float       gradient_scale = 0.05f;
  hmap::Array c = hmap::select_elevation_slope(z, gradient_scale);

  z.to_png("ex_select_elevation_slope0.png", hmap::Cmap::JET);
  c.to_png("ex_select_elevation_slope1.png", hmap::Cmap::JET);
}
