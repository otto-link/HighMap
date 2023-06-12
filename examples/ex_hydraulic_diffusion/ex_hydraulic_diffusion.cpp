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

  float c_diffusion = 0.1f;
  int   iterations = 100;

  auto  dn = gradient_norm(z);
  float talus_ref = 1.1f * dn.max();

  hmap::hydraulic_diffusion(z, c_diffusion, talus_ref, iterations);

  hmap::export_banner_png("ex_hydraulic_diffusion.png",
                          {z0, z},
                          hmap::cmap::terrain,
                          true);
}
