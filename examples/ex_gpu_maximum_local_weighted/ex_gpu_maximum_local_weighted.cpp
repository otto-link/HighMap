#include "highmap.hpp"
#include "highmap/dbg.hpp"

int main(void)
{
#ifdef ENABLE_OPENCL
  hmap::Vec2<int>   shape = {512, 256};
  hmap::Vec2<float> res = {8.f, 4.f};
  int               seed = 1;

  hmap::gpu::OpenCLConfig gpu_config;
  // gpu_config.infos();

  hmap::Timer timer = hmap::Timer("main");

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::n_perlin, shape, res, seed);
  hmap::remap(z);

  // CPU
  int         ir = 32;
  hmap::Array z1 = z;
  hmap::expand(z1, ir);

  // GPU
  hmap::Vec2<int> size = {ir * 2, ir * 2};
  hmap::Array     kernel = hmap::cubic_pulse(size);
  hmap::Array     z2;

  for (int bsize = 4; bsize <= 32; bsize *= 2)
  {
    LOG_INFO("BLOCK_SIZE: %d", bsize);
    gpu_config.set_block_size(bsize);

    z2 = z;
    hmap::gpu::maximum_local_weighted(gpu_config, z2, kernel);
  }

  hmap::export_banner_png("ex_gpu_maximum_local_weighted.png",
                          {z, z1, z2},
                          hmap::cmap::inferno);
#else
  LOG_ERROR("OpenCL not activated");
#endif
}
