#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z1 = hmap::worley_double(shape, kw, seed);

  hmap::Array z2 = hmap::fbm_worley_double(shape, kw, seed);

  hmap::export_banner_png("ex_worley_double.png",
                          {z1, z2},
                          hmap::cmap::viridis);
}
