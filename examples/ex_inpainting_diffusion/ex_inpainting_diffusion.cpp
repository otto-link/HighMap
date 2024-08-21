#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z1);

  hmap::Array mask = hmap::select_interval(z1, 0.5f, 0.6f);

  // heightmap with "holes"
  hmap::Array z2 = hmap::Array(shape);

  for (int i = 0; i < shape.x; i++)
    for (int j = 0; j < shape.y; j++)
      if (mask(i, j) == 0.f) z2(i, j) = z1(i, j);

  // holes filled with inpainting
  int         iterations = 200;
  hmap::Array z3 = hmap::inpainting_diffusion(z2, mask, iterations);

  hmap::export_banner_png("ex_inpainting_diffusion.png",
                          {z1, z2, z3},
                          hmap::Cmap::JET);
}
