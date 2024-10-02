#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z0 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z0);

  hmap::Array z1 = z0;
  hmap::remap(z1, 0.f, 0.5f);

  int         ir = 32;
  hmap::Array z2 = hmap::diffusion_retargeting(z0, z1, ir);

  hmap::export_banner_png("ex_diffusion_retargeting.png",
                          {z0, z1, z2},
                          hmap::Cmap::TERRAIN);
}
