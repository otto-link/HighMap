#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  glm::ivec2    shape = {1024, 1024};
  std::uint32_t seed = 0;

  // Generate a procedural mountainous terrain
  hmap::Array z = hmap::gpu::mountain_stump(shape, seed);

  // Apply some hydraulic erosion to generate deposition and flow channels
  hmap::Array deposition(shape, 0.f);
  hmap::gpu::hydraulic_particle(z, 200000, seed, nullptr, nullptr, &deposition);

  hmap::TerrainTextureParams params;

  hmap::Texture tex = hmap::terrain_texture(z, &deposition, params);
  tex.to_png("ex_terrain_texture_auto.png");

  z.dump();

  return 0;
}
