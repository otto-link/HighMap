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
  hmap::Array z1 = hmap::gpu::gavoronoise(shape, kw, seed, amp);

  float       angle = 45.f;
  float       angle_spread_ratio = 0.f;
  hmap::Array z2 = hmap::gpu::gavoronoise(shape,
                                          kw,
                                          seed,
                                          amp,
                                          angle,
                                          angle_spread_ratio);

  // with input base noise
  int         octaves = 2;
  hmap::Array base = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                     shape,
                                     kw,
                                     ++seed,
                                     octaves);

  // base amplitude amplitude expected to be in [-1, 1] (approx.)
  hmap::remap(base, -1.f, 1.f);
  hmap::Array z3 = hmap::gpu::gavoronoise(base, kw, seed, amp);

  z2.to_png_grayscale("out2.png", CV_16U);

  hmap::export_banner_png("ex_gavoronoise.png",
                          {z1, z2, z3},
                          hmap::Cmap::JET,
                          true);

#else
  std::cout << "OpenCL not activated\n";
#endif
}
