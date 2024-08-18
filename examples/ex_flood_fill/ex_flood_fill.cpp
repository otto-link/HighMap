#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  hmap::Array z = hmap::gaussian_pulse(shape, 32);
  hmap::remap(z);

  z = hmap::select_interval(z, 0.5f, 0.55f);

  auto z1 = z;
  hmap::flood_fill(z1, 128, 128);

  auto z2 = z;
  hmap::flood_fill(z2, 0, 0, 0.5f);

  z2.infos();

  hmap::export_banner_png("ex_flood_fill.png", {z, z1, z2}, hmap::Cmap::GRAY);
}
