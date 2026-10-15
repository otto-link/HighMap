#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  glm::ivec2 shape = {512, 512};
  glm::vec2  res = {3.f, 3.f};
  int        seed = 101;

  // Base terrain elevation used to compute flow/gradient
  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  // Generate a multi-channel RGB texture (color bands or procedural pattern)
  hmap::Array r = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, {16.f, 16.f}, seed + 1);
  hmap::Array g = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, {16.f, 16.f}, seed + 2);
  hmap::Array b = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, {16.f, 16.f}, seed + 3);

  hmap::remap(r);
  hmap::remap(g);
  hmap::remap(b);

  hmap::Texture tex(r, g, b);

  // 1. Single-scale texture advection
  int nparticles = 30000;
  int iterations = 4;

  hmap::Texture tex_advected = hmap::gpu::advection_particle(
      z,
      tex,
      iterations,
      nparticles,
      seed);

  // 2. Multiscale texture advection
  std::vector<int> steps_per_level = {4, 2, 1};
  float            particles_ratio = 0.25f;
  float            advection_length = 0.1f;
  float            value_persistence = 0.99f;
  float            inertia = 0.1f;
  float            mix = 1.f;
  float            warp = 0.15f;

  hmap::Texture tex_multiscale = hmap::gpu::advection_particle_multiscale(
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

  // Export resulting textures
  tex.to_png("ex_texture_advection_original.png");
  tex_advected.to_png("ex_texture_advection_single.png");
  tex_multiscale.to_png("ex_texture_advection_multiscale.png");

  return 0;
}
