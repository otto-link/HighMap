#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::remap(z);
  auto z1 = z;
  auto z2 = z;

  hmap::Array talus_map = hmap::Array(shape, 2.f / shape.x);

  hmap::gpu::thermal_ridge(z1, talus_map, 500);
  hmap::gpu::thermal_inflate(z2, talus_map, 500);

  z2.dump();

  hmap::export_banner_png("ex_thermal_ridge.png",
                          {z, z1, z2},
                          hmap::Cmap::TERRAIN,
                          true);
}
