#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;
  int               radius = 32;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::Array z0 = z;

  hmap::smooth_fill(z, radius);

  hmap::export_banner_png("ex_smooth_fill.png",
                          {z0, z},
                          hmap::Cmap::TERRAIN,
                          true);
  z.to_file("out.bin");
}
