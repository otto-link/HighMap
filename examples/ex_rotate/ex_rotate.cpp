#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto z1 = z;
  hmap::rotate(z1, 30.f);

  // gpu version
  hmap::gpu::init_opencl();
  auto z2 = z;
  hmap::gpu::rotate(z2, 30.f);

  auto z3 = z;
  hmap::gpu::rotate(z3, 30.f, false); // no zoom

  hmap::export_banner_png("ex_rotate.png", {z, z1, z2, z3}, hmap::Cmap::JET);
}
