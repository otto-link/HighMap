#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array w = hmap::fbm_perlin(shape, res, seed, 2);
  w = hmap::gradient_norm(w);
  hmap::remap(w, 1.f, 0.f);

  std::vector<float> amp(8);
  for (size_t k = 0; k < amp.size(); k++)
    amp[k] = std::pow(0.5f, k);

  hmap::Array z = hmap::fbm_perlin_advanced(shape, res, seed, amp, 2.f, &w);

  z.to_png("ex_fbm_perlin_advanced.png", hmap::cmap::terrain, true);
}
