#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  float radius = 48.f;
  float sigma_inner = 8.f;
  float sigma_outer = 32.f;
  float z_bottom = 0.3f;

  // without noise
  hmap::Array z1 = hmap::caldera(shape,
                                 radius,
                                 sigma_inner,
                                 sigma_outer,
                                 z_bottom);

  // with noise
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array noise = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                      shape,
                                      res,
                                      seed);
  float       noise_r_amp = 8.f;    // pixels
  float       noise_z_ratio = 0.4f; // in [0, 1]

  hmap::remap(noise);

  hmap::Array z2 = hmap::caldera(shape,
                                 radius,
                                 sigma_inner,
                                 sigma_outer,
                                 z_bottom,
                                 &noise,
                                 noise_r_amp,
                                 noise_z_ratio);

  hmap::export_banner_png("ex_caldera.png",
                          {z1, z2},
                          hmap::Cmap::TERRAIN,
                          true);
}
