#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  hmap::Array z = hmap::gabor(shape, 10.f, 30.f);
  hmap::Array k = hmap::curvature_gaussian(z);
  hmap::Array h = hmap::curvature_mean(z);

  z.to_png("ex_curvature_gaussian0.png", hmap::cmap::viridis);
  k.to_png("ex_curvature_gaussian1.png", hmap::cmap::viridis);
  h.to_png("ex_curvature_gaussian2.png", hmap::cmap::viridis);
}
