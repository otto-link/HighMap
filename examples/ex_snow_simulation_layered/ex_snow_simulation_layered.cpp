#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  glm::ivec2 shape = {256, 256};
  shape = {1024, 1024};
  glm::vec2 kw = {2.f, 2.f};
  int       seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);

  hmap::Array snow_depth = hmap::gpu::snow_simulation_layered(
      z,
      0.05f,
      hmap::Array(shape, 0.5f / shape.x));

  hmap::export_banner_png("ex_snow_simulation_layered.png",
                          {z, z + snow_depth, snow_depth},
                          hmap::Cmap::TERRAIN,
                          true);
}
