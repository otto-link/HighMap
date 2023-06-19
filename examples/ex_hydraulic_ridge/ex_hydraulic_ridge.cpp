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

  float talus = 16.f / shape[0];
  float intensity = 0.5f;

  // no smoothing
  auto  z1 = z;
  float smoothing_factor = 1.f;
  hmap::hydraulic_ridge(z1, talus, intensity, smoothing_factor);

  // lots of smoothing
  auto z2 = z;
  smoothing_factor = 0.05f;
  hmap::hydraulic_ridge(z2, talus, intensity, smoothing_factor);

  // can be combined with thermal erosion to yield smoother valley
  // bottoms
  auto z3 = z2;
  hmap::thermal_auto_bedrock(z3, 0.5f / shape[0]);

  hmap::export_banner_png("ex_hydraulic_ridge.png",
                          {z, z1, z2, z3},
                          hmap::cmap::terrain,
                          true);
}
