#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  // select angle 90° +- 30°
  float angle = 90.f;
  float sigma = 30.f;

  hmap::Array c = hmap::select_angle(z, angle, sigma);

  hmap::export_banner_png("ex_select_angle.png", {z, c}, hmap::Cmap::JET);
}
