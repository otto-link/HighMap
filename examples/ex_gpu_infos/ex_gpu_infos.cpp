#include <iostream>

#include "highmap.hpp"

int main(void)
{
#ifdef ENABLE_OPENCL
  hmap::gpu::OpenCLConfig gpu_config;
  gpu_config.infos();
#else
  std::cout << "OpenCL not activated\n";
#endif
}
