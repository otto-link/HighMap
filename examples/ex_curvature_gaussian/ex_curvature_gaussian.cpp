#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  hmap::Array z = hmap::gabor(shape, 4.f, 30.f);
  hmap::Array k = hmap::curvature_gaussian(z);
  hmap::Array h = hmap::curvature_mean(z);

  int         ir = 4;
  hmap::Array si = hmap::shape_index(z, ir);

  z.to_png("ex_curvature_gaussian0.png", hmap::cmap::terrain);
  k.to_png("ex_curvature_gaussian1.png", hmap::cmap::jet);
  h.to_png("ex_curvature_gaussian2.png", hmap::cmap::jet);

  si.to_png("ex_curvature_gaussian3.png", hmap::cmap::jet);
}
