#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array noise = 0.2f * hmap::fbm_perlin(shape, kw, seed);

  hmap::Array z1 = hmap::value_noise_thinplate(shape, kw, seed);
  hmap::Array z2 = hmap::value_noise_thinplate(shape, kw, seed, &noise);

  hmap::export_banner_png("ex_value_noise_thinplate.png",
                          {z1, z2},
                          hmap::cmap::inferno);
}
