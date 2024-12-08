#include <iostream>

#include "highmap.hpp"

int main(void)
{
#ifdef ENABLE_OPENCL
  hmap::Vec2<int> shape = {256, 512};
  int             seed = 1;

  hmap::gpu::OpenCLConfig gpu_config;
  // gpu_config.infos();

  hmap::Array z = hmap::white(shape, 0.f, 1.f, seed);
  hmap::remap(z);

  // CPU
  hmap::Array z1 = z;
  hmap::Timer::Start("full CPU");
  hmap::median_3x3(z1);
  hmap::Timer::Stop("full CPU");

  // GPU
  hmap::Array z2;

  for (int bsize = 1; bsize <= 32; bsize *= 2)
  {
    LOG_INFO("BLOCK_SIZE: %d", bsize);
    gpu_config.set_block_size(bsize);

    z2 = z;
    hmap::Timer::Start("full GPU");
    hmap::gpu::median_3x3(gpu_config, z2);
    hmap::Timer::Stop("full GPU");

    // z2 = z;
    // hmap::Timer::Start("full GPU img");
    // hmap::gpu::median_3x3_img(gpu_config, z2);
    // hmap::Timer::Stop("full GPU img");

    z2.infos();
  }

  hmap::export_banner_png("ex_gpu_median_3x3.png",
                          {z, z1, z2},
                          hmap::Cmap::INFERNO);
#else
  std::cout << "OpenCL not activated\n";
#endif
}
