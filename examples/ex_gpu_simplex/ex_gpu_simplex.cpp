#include <iostream>

#include "highmap.hpp"

int main(void)
{
#ifdef ENABLE_OPENCL
  hmap::Vec2<int>   shape = {256, 512};
  hmap::Vec2<float> kw = {2.f, 4.f};
  int               seed = 1;

  hmap::gpu::OpenCLConfig gpu_config;

  hmap::Timer::Start("full CPU");
  hmap::Array z1 = hmap::noise(hmap::NoiseType::SIMPLEX2S, shape, kw, seed);
  hmap::Timer::Stop("full CPU");

  hmap::Timer::Start("full GPU");
  hmap::Array z2 = hmap::gpu::simplex(gpu_config, shape, kw, seed);
  hmap::Timer::Stop("full GPU");

  hmap::remap(z1);
  hmap::remap(z2);

  z2.infos();

  hmap::export_banner_png("ex_gpu_simplex.png", {z1, z2}, hmap::Cmap::VIRIDIS);
#else
  std::cout << "OpenCL not activated\n";
#endif
}
