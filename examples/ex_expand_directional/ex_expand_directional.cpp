#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  int   ir = 64;
  float angle = 30.f;
  float aspect_ratio = 0.2f;

  hmap::Array z1 = z;
  hmap::expand_directional(z1, ir, angle, aspect_ratio);

  hmap::Array z2 = z;
  hmap::shrink_directional(z2, ir, angle, aspect_ratio);

  hmap::remap(z);
  hmap::remap(z1);
  hmap::remap(z2);

  hmap::export_banner_png("ex_expand_directional.png",
                          {z, z1, z2},
                          hmap::Cmap::TERRAIN);
}
