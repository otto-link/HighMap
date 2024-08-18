#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array noise = 0.5f * hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                             shape,
                                             res,
                                             seed + 1);

  hmap::Array dx = hmap::Array(shape);
  hmap::Array dy = hmap::Array(shape);

  hmap::Array z0 = hmap::noise(hmap::NoiseType::PERLIN, shape, res, seed);

  hmap::swirl(dx, dy, 1.f, 1.f);
  hmap::Array z1 = hmap::noise(hmap::NoiseType::PERLIN,
                               shape,
                               res,
                               seed,
                               &dx,
                               &dy);

  hmap::swirl(dx, dy, 1.f, 1.f, &noise);
  hmap::Array z2 = hmap::noise(hmap::NoiseType::PERLIN,
                               shape,
                               res,
                               seed,
                               &dx,
                               &dy);

  hmap::export_banner_png("ex_swirl.png", {z0, z1, z2}, hmap::Cmap::INFERNO);
}
