#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  glm::ivec2 shape = {256, 256};
  glm::vec2  kw = {4.f, 4.f};
  int        seed = 1;

  hmap::Array z0 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);

  auto z1 = z0;
  auto z2 = z0;

  hmap::gpu::thermal(z1, 0.1f / shape.x, 500);
  hmap::gpu::thermal_conserve(z2, 0.1f / shape.x, 500);

  hmap::log::trace("z0 sum: {}", z0.sum());
  hmap::log::trace("z0 sum: {}", z1.sum());
  hmap::log::trace("z0 sum: {}", z2.sum());

  hmap::export_banner_png("ex_thermal.png",
                          {z0, z1, z2},
                          hmap::Cmap::TERRAIN,
                          true);
}
