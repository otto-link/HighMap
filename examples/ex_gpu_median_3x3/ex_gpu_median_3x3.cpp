#include "highmap.hpp"
#include "highmap/dbg.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 512};
  int             seed = 1;

  hmap::gpu::OpenCLConfig gpu_config;
  // gpu_config.infos();

  hmap::Timer timer = hmap::Timer("main");

  hmap::Array z = hmap::white(shape, 0.f, 1.f, seed);
  hmap::remap(z);

  // CPU
  hmap::Array z1 = z;
  timer.start("full CPU");
  hmap::median_3x3(z1);
  timer.stop("full CPU");

  // GPU
  hmap::Array z2;

  for (int bsize = 1; bsize <= 32; bsize *= 2)
  {
    LOG_INFO("BLOCK_SIZE: %d", bsize);
    gpu_config.set_block_size(bsize);

    z2 = z;
    timer.start("full GPU");
    hmap::gpu::median_3x3(gpu_config, z2);
    timer.stop("full GPU");

    z2 = z;
    timer.start("full GPU img");
    hmap::gpu::median_3x3_2(gpu_config, z2);
    timer.stop("full GPU img");

    z2.infos();
  }

  hmap::export_banner_png("ex_gpu_median_3x3.png",
                          {z, z1, z2},
                          hmap::cmap::inferno);
}
