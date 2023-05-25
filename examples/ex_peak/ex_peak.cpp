#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  float radius = 64.f;

  hmap::Array noise = hmap::fbm_perlin(shape, res, seed);
  float       noise_r_amp = 16.f;   // pixels
  float       noise_z_ratio = 0.4f; // in [0, 1]

  hmap::remap(noise);

  hmap::Array z = hmap::peak(shape, radius, noise, noise_r_amp, noise_z_ratio);

  z.to_png("ex_peak.png", hmap::cmap::terrain, true);
}
