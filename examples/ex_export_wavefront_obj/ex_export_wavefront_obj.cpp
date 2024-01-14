#include "highmap.hpp"

int main(void)
{

  const hmap::Vec2<int>   shape = {256, 256};
  const hmap::Vec2<float> res = {2.f, 2.f};
  int                     seed = 1;

  hmap::Array z = hmap::fbm_simplex(shape, res, seed);
  hmap::remap(z);

  z.to_png("out.png", hmap::cmap::terrain, true);
  hmap::export_wavefront_obj("hmap.obj", z);
}
