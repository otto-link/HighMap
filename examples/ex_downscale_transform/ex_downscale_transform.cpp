#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 2;

  hmap::Array z128 = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                     {128, 128},
                                     kw,
                                     seed);
  hmap::Array z256 = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                     {256, 256},
                                     kw,
                                     seed);
  hmap::Array z512 = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                     {512, 512},
                                     kw,
                                     seed);
  hmap::Array z1024 = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                      {1024, 1024},
                                      kw,
                                      seed);
  auto        z0 = z1024; // keep for reference

  float kc = 64.f;

  auto lambda = [](hmap::Array &x)
  {
    int  nparticles = 5000;
    uint seed = 0;
    hmap::hydraulic_particle(x, nparticles, seed);
  };

  // apply the erosion to each array with different resolutions
  // (results should be the same)
  hmap::downscale_transform(z128, kc, lambda);
  hmap::downscale_transform(z256, kc, lambda);
  hmap::downscale_transform(z512, kc, lambda);
  hmap::downscale_transform(z1024, kc, lambda);

  // interpolation in finer resolution to generate a single output image
  auto z1 = z128.resample_to_shape({1024, 1024});
  auto z2 = z256.resample_to_shape({1024, 1024});
  auto z3 = z512.resample_to_shape({1024, 1024});
  auto z4 = z1024.resample_to_shape({1024, 1024});

  hmap::export_banner_png("ex_downscale_transform.png",
                          {z0, z1, z2, z3, z4},
                          hmap::Cmap::TERRAIN,
                          true);
}
