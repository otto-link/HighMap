#include "highmap.hpp"
#include "highmap/dbg.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 512};
  hmap::Vec2<float> kw = {2.f, 4.f};
  int               seed = 1;

  hmap::gpu::OpenCLConfig gpu_config;

  hmap::Timer timer = hmap::Timer("main");

  timer.start("full CPU");
  auto z1 = hmap::simplex(shape, kw, seed);
  timer.stop("full CPU");

  timer.start("full GPU");
  auto z2 = hmap::gpu::simplex(gpu_config, shape, kw, seed);
  timer.stop("full GPU");

  hmap::remap(z1);
  hmap::remap(z2);

  z2.infos();

  hmap::export_banner_png("ex_gpu_simplex.png", {z1, z2}, hmap::cmap::viridis);
}
