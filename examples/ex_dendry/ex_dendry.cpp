#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  auto control_hmap = hmap::perlin(shape, kw, seed);
  hmap::remap(control_hmap, 0.5f, 1.f);

  hmap::Vec2<float> kd = {8.f, 8.f};
  auto              z = hmap::dendry(shape, kd, seed, control_hmap);
  hmap::remap(z);

  hmap::export_banner_png("ex_dendry.png",
                          {control_hmap, z},
                          hmap::cmap::viridis);
}
