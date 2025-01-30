#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  auto        z0 = z;
  hmap::remap(z);

  hmap::Array talus_map = hmap::Array(shape, 2.f / shape.x);

  hmap::gpu::thermal_ridge(z, talus_map, 500);

  hmap::export_banner_png("ex_thermal_ridge.png",
                          {z0, z},
                          hmap::Cmap::TERRAIN,
                          true);
}
