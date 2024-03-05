#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array noise = 0.2f * hmap::fbm_perlin(shape, res, seed);

  hmap::Array z1 = hmap::value_noise_linear(shape, res, seed);

  hmap::export_banner_png("ex_value_noise_linear.png",
                          {z1},
                          hmap::cmap::inferno);
}
