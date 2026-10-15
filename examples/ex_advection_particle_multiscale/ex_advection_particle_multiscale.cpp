#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  glm::ivec2 shape = {512, 512};
  glm::vec2  res = {3.f, 3.f};
  int        seed = 42;

  // Base terrain elevation
  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  // Field to advect: high-frequency texture / noise pattern
  hmap::Array tex = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                    shape,
                                    {24.f, 24.f},
                                    seed + 1);

  // 1. Single-scale particle advection
  int nparticles = 30000;
  int iterations = 4;

  hmap::Array tex_singlescale = hmap::gpu::advection_particle(z,
                                                             tex,
                                                             iterations,
                                                             nparticles,
                                                             seed);

  // 2. Multiscale particle advection
  std::vector<int> steps_per_level = {4, 2, 1};
  float            particles_ratio = 0.25f;
  float            advection_length = 0.1f;
  float            value_persistence = 0.99f;
  float            inertia = 0.1f;
  float            mix = 1.f;
  float            warp = 0.15f;

  hmap::Array tex_multiscale = hmap::gpu::advection_particle_multiscale(
      z,
      tex,
      seed,
      steps_per_level,
      particles_ratio,
      advection_length,
      value_persistence,
      inertia,
      mix,
      warp);

  hmap::export_banner_png("ex_advection_particle_multiscale.png",
                          {z, tex, tex_singlescale, tex_multiscale},
                          hmap::Cmap::VIRIDIS,
                          true);

  return 0;
}
