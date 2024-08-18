#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 1;

  float scale = 1.f / 128.f;

  hmap::Array z = hmap::diffusion_limited_aggregation(shape, scale, seed);

  z.to_png("ex_diffusion_limited_aggregation.png", hmap::Cmap::TERRAIN, false);
}
