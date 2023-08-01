#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  auto dz = hmap::gradient_norm(z);
  dz.to_png("ex_gradient_norm.png", hmap::cmap::viridis);
}
