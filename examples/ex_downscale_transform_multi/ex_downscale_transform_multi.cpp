#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 2;

  hmap::Array z512 = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                     {512, 512},
                                     kw,
                                     seed);
  hmap::Array z1024 = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                      {1024, 1024},
                                      kw,
                                      seed);
  auto        z0 = z1024; // keep for reference

  std::vector<float> kc_list = {64.f, 96.f, 256.f};
  // std::vector<float> kc_list = {256.f, 96.f, 64.f};

  auto lambda = [](hmap::Array &x, const int current_index)
  {
    int  nparticles = 5000;
    uint seed = 0;
    hmap::hydraulic_particle(x, nparticles, seed);
  };

  // apply the erosion to each array with different resolutions
  // (results should be the same)
  hmap::downscale_transform_multi(z512, kc_list, lambda);
  hmap::downscale_transform_multi(z1024, kc_list, lambda);

  // interpolation in finer resolution to generate a single output image
  auto z3 = z512.resample_to_shape({1024, 1024});
  auto z4 = z1024.resample_to_shape({1024, 1024});

  hmap::export_banner_png("ex_downscale_transform_multi.png",
                          {z0, z3, z4},
                          hmap::Cmap::TERRAIN,
                          true);
}
