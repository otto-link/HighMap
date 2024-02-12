#include "highmap.hpp"
#include "highmap/dbg.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  hmap::gpu::OpenCLConfig gpu_config;

  // "segments", coordinates are given by pairs (xa_1, xa_2, xb_1,
  // xb_2, ...)
  std::vector<float> x = {0.1f, 0.5f, 0.7f, 0.2f, 0.2f, 0.1f};
  std::vector<float> y = {0.2f, 0.2f, 0.8f, 0.9f, 0.9f, 0.6f};
  std::vector<float> v = {1.0f, 0.5f, 0.8f, 0.4f, 0.4f, 0.9f};

  float slope = 3.f;

  auto z = hmap::gpu::ridgelines(gpu_config, shape, x, y, v, slope);
  hmap::remap(z);

  hmap::export_banner_png("ex_gpu_ridgelines.png", {z}, hmap::cmap::inferno);
}
