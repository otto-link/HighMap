#include "highmap.hpp"

int main(void)
{

  const std::vector<int>   shape = {512, 512};
  const std::vector<float> res = {0.5f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::perlin(shape, res, seed);

  std::vector<int> tiling = {4, 2};
  hmap::HeightMap  h = hmap::HeightMap(shape, tiling);

  h.infos();

  // auto lambda = [&res, &seed](std::vector<int> shape, std::vector<float>
  // shift) { return hmap::perlin(shape, res, seed, shift); };

  auto lambda = [&res, &seed](std::vector<int> shape, std::vector<float> shift)
  { return hmap::fbm_perlin(shape, res, seed, 8, 0.7f, 0.5f, 2.f, shift); };

  hmap::fill(h, lambda);

  hmap::transform1(h, [](hmap::Array &x) { hmap::gamma_correction(x, 2.f); });
  hmap::gamma_correction(z, 2.f);

  z.to_png("out.png", hmap::cmap::inferno);

  h.to_array().to_png("out_h.png", hmap::cmap::inferno);
}
