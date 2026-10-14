#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  glm::ivec2 shape = {256, 256};
  glm::vec2  kw = {3.f, 3.f};
  int        seed = 0;

  // --- Base input: stepped / flat plateau shapes from quantized noise or
  // Voronoi

  hmap::Array z0 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);

  const float slope = 2.f / shape.x;

  // --- 1. Symmetrical slopes (Pyramid mode)

  hmap::Array z_pyramid = hmap::gpu::distance_shaper(
      z0,
      slope,
      8,
      /* skew */ 0.f,
      /* angle */ 0.f,
      hmap::DistanceMode::PYRAMID);

  // --- 2. Asterisk mode (cutting into faces)

  hmap::Array z_asterisk = hmap::gpu::distance_shaper(
      z0,
      slope,
      8,
      /* skew */ 0.f,
      /* angle */ 0.f,
      hmap::DistanceMode::ASTERISK);

  // --- 3. Skewed slopes (tilted stratified formations)

  hmap::Array z_skewed = hmap::gpu::distance_shaper(
      z0,
      slope,
      8,
      /* skew */ 0.8f,
      /* angle */ 35.f,
      hmap::DistanceMode::PYRAMID);

  // --- 4. Folded / terraced rock formation with warp displacement

  hmap::Array dx = 0.05f * hmap::noise_fbm(hmap::NoiseType::SIMPLEX2,
                                           shape,
                                           {4.f, 4.f},
                                           seed + 10);
  hmap::Array dy = 0.05f * hmap::noise_fbm(hmap::NoiseType::SIMPLEX2,
                                           shape,
                                           {4.f, 4.f},
                                           seed + 20);

  hmap::Array z_folded = hmap::gpu::distance_shaper(
      z0,
      slope,
      8,
      /* skew */ 0.7f,
      /* angle */ 45.f,
      hmap::DistanceMode::ASTERISK,
      hmap::DistanceMethod::RT,
      0.1f,
      0.f,
      0.f,
      false,
      false,
      0.9f,
      seed,
      nullptr,
      &dx,
      &dy);

  hmap::export_banner_png("ex_distance_shaper.png",
                          {z0, z_pyramid, z_asterisk, z_skewed, z_folded},
                          hmap::Cmap::TERRAIN,
                          true);

  return 0;
}
