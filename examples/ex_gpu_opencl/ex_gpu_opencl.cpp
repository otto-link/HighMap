#include "highmap.hpp"

#include "highmap/dbg/assert.hpp"

#include <iostream>

#ifdef ENABLE_OPENCL
template <typename F1, typename F2>
void compare(F1 fct1, F2 fct2, float tolerance, const std::string &fname)
{
  hmap::Vec2<int> shape = {256, 512};
  shape = {512, 512};
  // shape = {1024, 1024};
  // shape = {2048, 2048};
  // shape = {4096 * 2, 4096 * 2};
  hmap::Vec2<float> kw = {2.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  // hmap::Array z = hmap::white(shape, 0.f, 1.f, seed);
  hmap::remap(z);

  hmap::Array z1 = z;
  hmap::Array z2 = z;

  // host
  hmap::Timer::Start("host");
  fct1(z1);
  hmap::Timer::Stop("host");

  z1.to_png_grayscale("out1.png", CV_16U);

  // GPU
  hmap::Timer::Start("GPU");
  fct2(z2);
  hmap::Timer::Stop("GPU");

  z2.to_png_grayscale("out2.png", CV_16U);

  hmap::AssertResults res;
  hmap::assert_almost_equal(z1, z2, tolerance, fname, &res);
  res.msg += "[" + fname + "]";
  res.print();
}
#endif

// ---

int main(void)
{

#ifdef ENABLE_OPENCL
  hmap::gpu::init_opencl();

  hmap::Vec2<int> shape = {256, 512};
  shape = {512, 512};
  // shape = {1024, 1024};

  clwrapper::KernelManager::get_instance().set_block_size(32);

  // compare([](hmap::Array &z) { hmap::median_3x3(z); },
  //         [](hmap::Array &z) { hmap::gpu::median_3x3(z); },
  //         1e-3f,
  //         "diff_median_3x3.png");

  int ir = 32;

  compare([ir](hmap::Array &z) { z = hmap::closing(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::closing(z, ir); },
          1e-3f,
          "closing.png");

  compare([ir](hmap::Array &z) { z = hmap::dilation(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::dilation(z, ir); },
          1e-3f,
          "dilation.png");

  compare([ir](hmap::Array &z) { z = hmap::erosion(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::erosion(z, ir); },
          1e-3f,
          "erosion.png");

  compare([ir](hmap::Array &z) { z = hmap::morphological_gradient(z, ir); },
          [ir](hmap::Array &z)
          { z = hmap::gpu::morphological_gradient(z, ir); },
          1e-3f,
          "morphological_gradient.png");

  compare([ir](hmap::Array &z) { z = hmap::opening(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::opening(z, ir); },
          1e-3f,
          "opening.png");

#else
  std::cout << "OpenCL not activated\n";
#endif
}
