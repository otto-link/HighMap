#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {1.f, 1.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  auto        z0 = z;

  float       c_erosion = 0.02f;
  float       talus_ref = 0.1f / (float)shape[0];
  int         iradius = 11;
  hmap::Array z_bedrock = hmap::minimum_local(z, iradius);

  hmap::hydraulic_stream(z, z_bedrock, c_erosion, talus_ref);

  hmap::export_banner_png("ex_hydraulic_stream.png", {z0, z}, hmap::cmap::gray);
}
