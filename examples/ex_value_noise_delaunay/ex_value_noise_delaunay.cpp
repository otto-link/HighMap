#include "highmap.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  const float            kw = 4.f;
  int                    seed = 1;

  hmap::Array noise = 0.2f * hmap::fbm_perlin(shape, {kw, kw}, seed);

  hmap::Array z1 = hmap::value_noise_delaunay(shape, kw, seed);
  hmap::Array z2 = hmap::value_noise_delaunay(shape, kw, seed, &noise);

  hmap::export_banner_png("ex_value_noise_delaunay.png",
                          {z1, z2},
                          hmap::cmap::inferno);
}
