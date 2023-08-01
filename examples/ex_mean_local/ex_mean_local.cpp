#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;
  int               radius = 10;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::Array zm = hmap::mean_local(z, radius);

  z.to_png("ex_mean_local0.png", hmap::cmap::viridis);
  zm.to_png("ex_mean_local1.png", hmap::cmap::viridis);
}
