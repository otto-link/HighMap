#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  int ir = 16;

  hmap::Array z1 = hmap::z_score(z, ir);
  hmap::remap(z1);

  // gpu version
  hmap::gpu::init_opencl();
  hmap::Array z2 = hmap::gpu::z_score(z, ir);
  hmap::remap(z2);

  hmap::export_banner_png("ex_z_score.png", {z, z1, z2}, hmap::Cmap::INFERNO);
}
