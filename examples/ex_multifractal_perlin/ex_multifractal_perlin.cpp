#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::multifractal_perlin(shape, res, seed);
  z.to_png("ex_multifractal_perlin.png", hmap::cmap::terrain, true);
}
