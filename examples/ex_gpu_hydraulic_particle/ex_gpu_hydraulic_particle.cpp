#include "highmap.hpp"
#include "highmap/dbg.hpp"

int main(void)
{
#ifdef ENABLE_OPENCL
  hmap::Vec2<int>   shape = {256, 512};
  hmap::Vec2<float> res = {2.f, 4.f};
  int               seed = 1;

  hmap::gpu::OpenCLConfig gpu_config;
  // gpu_config.infos();

  hmap::Timer timer = hmap::Timer("main");

  hmap::Array z = hmap::fbm_simplex(shape, res, seed);
  hmap::remap(z);

  int nparticles = 100000;

  hmap::Array z1;
  for (int bsize = 64; bsize <= 64; bsize *= 2)
  {
    LOG_INFO("BLOCK_SIZE: %d", bsize);
    gpu_config.set_block_size(bsize);

    z1 = z;

    timer.start("full GPU");
    hmap::gpu::hydraulic_particle(gpu_config, z1, nparticles, seed);
    timer.stop("full GPU");

    timer.start("median filter GPU");
    hmap::gpu::median_3x3_img(gpu_config, z1);
    timer.stop("median filter GPU");
  }

  hmap::export_banner_png("ex_gpu_hydraulic_particle.png",
                          {z, z1},
                          hmap::cmap::terrain,
                          true);
#else
  LOG_ERROR("OpenCL not activated");
#endif
}
