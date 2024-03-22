#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::n_perlin, shape, res, seed);
  auto        z1 = z;
  auto        z2 = z;

  float scale = 4.f; // not in pixels
  hmap::steepen(z1, scale);

  // using a negative scale will "flatten" the map
  hmap::steepen(z2, -scale);

  hmap::export_banner_png("ex_steepen.png", {z, z1, z2}, hmap::cmap::terrain);
}
