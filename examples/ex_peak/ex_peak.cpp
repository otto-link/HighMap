#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  float radius = 64.f;

  hmap::Array noise = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                      shape,
                                      res,
                                      seed);
  float       noise_r_amp = 16.f;   // pixels
  float       noise_z_ratio = 0.4f; // in [0, 1]

  hmap::remap(noise);

  hmap::Array z = hmap::peak(shape, radius, &noise, noise_r_amp, noise_z_ratio);

  z.to_png("ex_peak.png", hmap::Cmap::TERRAIN, true);
}
