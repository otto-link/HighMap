#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  float             particle_density = 0.1f;
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  auto z1 = z;
  hmap::hydraulic_particle_multiscale(z1, particle_density, seed);

  auto moisture_map = z * z;
  auto z2 = z;
  hmap::hydraulic_particle_multiscale(z2,
                                      particle_density,
                                      seed,
                                      nullptr,
                                      &moisture_map);

  auto        z3 = z;
  hmap::Array erosion_map;
  hmap::Array deposition_map;
  hmap::hydraulic_particle_multiscale(z3,
                                      particle_density,
                                      seed,
                                      nullptr,
                                      &moisture_map,
                                      &erosion_map,
                                      &deposition_map);

  hmap::export_banner_png("ex_hydraulic_particle_multiscale0.png",
                          {z, z1},
                          hmap::Cmap::TERRAIN,
                          true);

  hmap::export_banner_png("ex_hydraulic_particle_multiscale1.png",
                          {erosion_map, deposition_map},
                          hmap::Cmap::INFERNO);
}
