#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  auto        z1 = z;
  auto        z2 = z;

  float scale = 4.f; // not in pixels
  hmap::steepen(z1, scale);

  // using a negative scale will "flatten" the map
  hmap::steepen(z2, -scale);

  hmap::export_banner_png("ex_steepen.png", {z, z1, z2}, hmap::cmap::terrain);
}
