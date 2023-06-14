#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  auto        z0 = z;

  hmap::thermal_flatten(z, 8.f / shape[0], 10);

  hmap::export_banner_png("ex_thermal_flatten.png",
                          {z0, z},
                          hmap::cmap::terrain,
                          true);
  z.to_file("out.bin");
}
