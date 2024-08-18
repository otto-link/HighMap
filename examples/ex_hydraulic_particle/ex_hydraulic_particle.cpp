#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               nparticles = 50000;
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  auto z1 = z;
  hmap::hydraulic_particle(z1, nparticles, seed);

  auto moisture_map = z * z;
  auto z2 = z;
  hmap::hydraulic_particle(z2, nparticles, seed, nullptr, &moisture_map);

  auto        z3 = z;
  hmap::Array erosion_map;
  hmap::Array deposition_map;
  hmap::hydraulic_particle(z3,
                           nparticles,
                           seed,
                           nullptr,
                           &moisture_map,
                           &erosion_map,
                           &deposition_map);

  hmap::export_banner_png("ex_hydraulic_particle0.png",
                          {z, z1, z2},
                          hmap::Cmap::TERRAIN,
                          true);

  hmap::export_banner_png("ex_hydraulic_particle1.png",
                          {erosion_map, deposition_map},
                          hmap::Cmap::INFERNO);
}
