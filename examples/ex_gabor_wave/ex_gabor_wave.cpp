#include "highmap.hpp"

int main(void)
{
#ifdef ENABLE_OPENCL
  hmap::gpu::init_opencl();

  hmap::Vec2<int> shape = {256, 256};
  shape = {1024, 1024};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  clwrapper::KernelManager::get_instance().set_block_size(32);

  hmap::Array z = hmap::gpu::gabor_wave(shape, kw, seed);
  hmap::Array z_fbm = hmap::gpu::gabor_wave_fbm(shape, kw, seed);

  hmap::export_banner_png("ex_gabor_wave.png", {z, z_fbm}, hmap::Cmap::JET);

  z_fbm.to_png_grayscale("out1.png", CV_16U);

#else
  std::cout << "OpenCL not activated\n";
#endif
}
