#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::Array z0 = z;

  hmap::Array dx = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                   shape,
                                   {4.f, 4.f},
                                   seed + 1);
  hmap::Array dy = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                   shape,
                                   {4.f, 4.f},
                                   seed + 2);

  // warping scale assuming a unit square domain
  float scale = 0.25f;
  remap(dx, -scale, scale);
  remap(dy, -scale, scale);

  hmap::warp(z, &dx, &dy);

  hmap::export_banner_png("ex_warp.png", {z0, z}, hmap::Cmap::VIRIDIS);
}
