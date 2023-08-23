#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {512, 512};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array w = hmap::constant(shape, 0.f);

  std::vector<float> amp(8);
  for (size_t k = 0; k < amp.size(); k++)
    amp[k] = std::pow(0.5f, k);

  hmap::Array z = hmap::fbm_perlin_advanced(shape, res, seed, amp, 2.f, &w);

  z.to_png("ex_fbm_perlin_advanced.png", hmap::cmap::terrain, true);
}
