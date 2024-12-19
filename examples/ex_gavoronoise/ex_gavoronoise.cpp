#include "highmap.hpp"

int main(void)
{
#ifdef ENABLE_OPENCL
  hmap::gpu::init_opencl();

  hmap::Vec2<int> shape = {256, 256};
  shape = {512, 512};
  shape = {1024, 1024};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  clwrapper::KernelManager::get_instance().set_block_size(32);

  float       amp = 0.025f;
  hmap::Array z = hmap::gpu::gavoronoise(shape, kw, seed, amp);

  hmap::export_banner_png("ex_gavoronoise.png", {z}, hmap::Cmap::JET, true);

#else
  std::cout << "OpenCL not activated\n";
#endif
}
