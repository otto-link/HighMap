#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               nparticles = 50000;
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  auto z_eroded = z;
  hmap::hydraulic_particle(z_eroded, nparticles, seed);

  hmap::Array erosion_map, deposition_map;

  hmap::erosion_maps(z, z_eroded, erosion_map, deposition_map);

  hmap::export_banner_png("ex_erosions_maps0.png",
                          {z, z_eroded},
                          hmap::Cmap::TERRAIN,
                          true);

  hmap::export_banner_png("ex_erosions_maps1.png",
                          {erosion_map, deposition_map},
                          hmap::Cmap::INFERNO);
}
