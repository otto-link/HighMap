#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {512, 512};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  auto        z0 = z;

  hmap::remap(z);
  // z = z * hmap::cone_smooth(z.shape);

  z0 = z;
  hmap::remap(z0);

  hmap::hydraulic_benes(z, 50);

  z.infos();
  remap(z);

  hmap::export_banner_png("ex_hydraulic_benes.png",
                          {z0, z},
                          hmap::cmap::terrain,
                          true);

  z.to_file("out.bin");
}
