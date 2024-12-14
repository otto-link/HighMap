#include "highmap.hpp"

int main(void)
{

#ifdef ENABLE_OPENCL
  hmap::Vec2<int> shape = {256, 512};
  // shape = {1024, 2048};
  int seed = 1;

  hmap::gpu::init_opencl();

  hmap::Array z = hmap::white(shape, 0.f, 1.f, seed);

  // for (int i = 0; i < shape.x; i++)
  //   for (int j = 0; j < shape.y; j++)
  //     z(i, j) = j;

  hmap::remap(z);

  // CPU
  hmap::Array z1 = z;
  hmap::Timer::Start("full CPU");
  hmap::median_3x3(z1);
  hmap::Timer::Stop("full CPU");

  // GPU
  hmap::Array z2 = z;
  hmap::Timer::Start("full GPU");
  hmap::gpu::median_3x3(z2);
  hmap::Timer::Stop("full GPU");

  hmap::Array z3 = z;
  hmap::Timer::Start("CPU");
  z3 = hmap::mean_local(z3, 32);
  hmap::Timer::Stop("CPU");

  hmap::Array z4 = z;
  hmap::Timer::Start("GPU");
  hmap::gpu::avg(z4, 32);
  hmap::Timer::Stop("GPU");

  z3.to_png("out1.png", hmap::Cmap::JET);
  z4.to_png("out2.png", hmap::Cmap::JET);

  // clwrapper::KernelManager::get_instance().set_block_size(32);
  // z2 = z;
  // hmap::Timer::Start("full GPU32");
  // hmap::gpu::median_3x3(z2);
  // hmap::Timer::Stop("full GPU32");

  hmap::export_banner_png("ex_gpu_opencl.png",
                          {z, z1, z2},
                          hmap::Cmap::INFERNO);
#else
  std::cout << "OpenCL not activated\n";
#endif
}
