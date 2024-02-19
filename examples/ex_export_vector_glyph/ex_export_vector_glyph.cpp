#include "highmap.hpp"

int main(void)
{

  const hmap::Vec2<int>   shape = {256, 256};
  const hmap::Vec2<float> res = {2.f, 2.f};
  int                     seed = 1;

  hmap::Array u = hmap::perlin(shape, res, ++seed);
  hmap::Array v = hmap::perlin(shape, res, ++seed);

  hmap::export_vector_glyph_png_16bit("ex_export_vector_glyph.png", u, v);
}
