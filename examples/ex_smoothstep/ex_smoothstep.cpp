#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  float       vmin = 0.25f;
  float       vmax = 0.75f;
  hmap::Array z3 = hmap::smoothstep3(z, vmin, vmax);
  hmap::Array z5 = hmap::smoothstep5(z, vmin, vmax);

  hmap::export_banner_png("ex_smoothstep.png",
                          {z, z3, z5},
                          hmap::Cmap::INFERNO);
}
