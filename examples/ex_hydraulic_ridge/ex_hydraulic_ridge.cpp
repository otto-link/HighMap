#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  float talus = 16.f / shape.x;
  float intensity = 0.5f;
  float erosion_factor = 1.5f;

  // no smoothing
  auto  z1 = z;
  float smoothing_factor = 1.f;
  hmap::hydraulic_ridge(z1, talus, intensity, erosion_factor, smoothing_factor);

  // lots of smoothing
  auto z2 = z;
  smoothing_factor = 0.05f;
  hmap::hydraulic_ridge(z2, talus, intensity, erosion_factor, smoothing_factor);

  // can be combined with thermal erosion to yield smoother valley
  // bottoms
  auto z3 = z2;
  hmap::thermal_auto_bedrock(z3, 0.5f / shape.x);

  // prefiltering to emphasize large scales
  auto z4 = z;
  int  ir = 8;
  smoothing_factor = 1.f;
  hmap::hydraulic_ridge(z4,
                        talus,
                        intensity,
                        erosion_factor,
                        smoothing_factor,
                        0.f,
                        ir);

  hmap::export_banner_png("ex_hydraulic_ridge.png",
                          {z, z1, z2, z3, z4},
                          hmap::cmap::terrain,
                          true);
}
