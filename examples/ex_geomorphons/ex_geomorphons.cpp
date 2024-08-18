#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);
  hmap::clamp_min_smooth(z, 0.3f, 0.1f);

  int   irmin = 1;
  int   irmax = 4;
  float epsilon = 0.001f;

  hmap::Array labels = hmap::geomorphons(z, irmin, irmax, epsilon);

  z.to_png("ex_geomorphons0.png", hmap::Cmap::TERRAIN);
  labels.to_png("ex_geomorphons1.png", hmap::Cmap::NIPY_SPECTRAL);
}
