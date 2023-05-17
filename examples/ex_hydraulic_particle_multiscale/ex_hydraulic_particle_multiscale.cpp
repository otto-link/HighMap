#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {2.f, 2.f};
  float particle_density = 0.3f;
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  auto        z0 = z;
  auto moisture_map = z;

  hmap::remap(moisture_map);

  hmap::hydraulic_particle_multiscale(z, moisture_map, particle_density, {8.f, 4.f, 2.f}, seed);

  hmap::export_banner_png("ex_hydraulic_particle_multiscale.png",
                          {z0, z},
                          hmap::cmap::terrain,
                          true);
}
