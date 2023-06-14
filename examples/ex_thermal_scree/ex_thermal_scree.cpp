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
  hmap ::remap(z);
  auto z0 = z;

  float talus = 2.f / shape[0];
  float noise_ratio = 0.5f;
  float zmin = -5.f; // == no limit
  float zmax = 0.5f;

  hmap::thermal_scree(z, talus, seed, noise_ratio, zmin, zmax);

  hmap::export_banner_png("ex_thermal_scree.png",
                          {z0, z},
                          hmap::cmap::terrain,
                          true);
  z.to_file("out.bin");
}
