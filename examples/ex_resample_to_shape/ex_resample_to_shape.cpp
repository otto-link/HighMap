#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::perlin(shape, res, seed);

  auto zr = z.resample_to_shape({32, 32});

  zr.to_png("ex_resample_to_shape.png", hmap::cmap::viridis);
}
