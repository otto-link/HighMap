#include "highmap.hpp"
#include "highmap/dbg.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::gpu::OpenCLConfig gpu_config;

  hmap::Timer timer = hmap::Timer("main");

  auto z1 = hmap::gpu::voronoise(gpu_config, shape, kw, 0.f, 0.f, seed);
  auto z2 = hmap::gpu::voronoise(gpu_config, shape, kw, 1.f, 0.f, seed);
  auto z3 = hmap::gpu::voronoise(gpu_config, shape, kw, 0.f, 1.f, seed);
  auto z4 = hmap::gpu::voronoise(gpu_config, shape, kw, 1.f, 1.f, seed);

  hmap::remap(z1);
  hmap::remap(z2);
  hmap::remap(z3);
  hmap::remap(z4);

  hmap::export_banner_png("ex_gpu_voronoise.png",
                          {z1, z2, z3, z4},
                          hmap::cmap::viridis);
}
