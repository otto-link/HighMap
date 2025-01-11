#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z00 = hmap::gpu::voronoise(shape, kw, 0.f, 0.f, seed);
  hmap::Array z10 = hmap::gpu::voronoise(shape, kw, 1.f, 0.f, seed);
  hmap::Array z01 = hmap::gpu::voronoise(shape, kw, 0.f, 1.f, seed);
  hmap::Array z11 = hmap::gpu::voronoise(shape, kw, 1.f, 1.f, seed);

  hmap::Array zfbm = hmap::gpu::voronoise_fbm(shape, kw, 1.f, 0.3f, seed);

  zfbm.infos();

  hmap::export_banner_png("ex_voronoise.png",
                          {z00, z10, z01, z11, zfbm},
                          hmap::Cmap::JET);
}
