#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 2;

  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, ++seed);
  hmap::Array z2 = 0.5f * hmap::noise_fbm(hmap::NoiseType::WORLEY,
                                          shape,
                                          2.f * kw,
                                          ++seed);

  int         iterations = 5000;
  hmap::Array z3 = hmap::gpu::blend_poisson_bf(z1, z2, iterations);

  hmap::remap(z1);
  hmap::remap(z2);
  hmap::remap(z3);

  hmap::export_banner_png("ex_blend_poisson_bf.png",
                          {z1, z2, z3},
                          hmap::Cmap::JET);
}
