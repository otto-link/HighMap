#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {512, 512};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 2;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::remap(z, 0.5f, 1.f);
  auto z0 = z;

  z0.to_png("ex_hydraulic_vpipes0.png", hmap::cmap::terrain);

  hmap::hydraulic_vpipes(z);

  z.infos();
  z.to_file("out.bin");

  hmap::export_banner_png("ex_hydraulic_vpipes.png", {z}, hmap::cmap::terrain);
}
