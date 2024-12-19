#include "highmap.hpp"

#include "highmap/dbg/assert.hpp"

#include <iostream>

#ifdef ENABLE_OPENCL
template <typename F1, typename F2>
void compare(F1 fct1, F2 fct2, float tolerance, const std::string &fname)
{
  hmap::Vec2<int> shape = {256, 512};
  shape = {512, 512};
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
  // shape = {512, 512};
  shape = {1024, 1024};

  clwrapper::KernelManager::get_instance().set_block_size(32);

  // compare([](hmap::Array &z) { hmap::median_3x3(z); },
  //         [](hmap::Array &z) { hmap::gpu::median_3x3(z); },
  //         1e-3f,
  //         "diff_median_3x3.png");

  // {
  //   hmap::Array talus(shape, 0.5f / shape.x);
  //   int         iterations = 5000;
  //   compare([&talus, &iterations](hmap::Array &z)
  //           { hmap::thermal(z, talus, 10); },
  //           [&talus, &iterations](hmap::Array &z)
  //           { hmap::gpu::thermal(z, talus, iterations); },
  //           1e-3f,
  //           "diff_thermal.png");
  // }

  // {
  //   int iterations = 10;
  //   compare([&iterations](hmap::Array &z) { hmap::thermal_rib(z, iterations);
  //   },
  //           [&iterations](hmap::Array &z)
  //           { hmap::gpu::thermal_rib(z, iterations); },
  //           1e-3f,
  //           "diff_thermal_rib.png");
  // }

  // {
  //   hmap::Array talus(shape, 2.f / shape.x);
  //   int         iterations = 100;
  //   compare(
  //       [&talus, &iterations](hmap::Array &z)
  //       {
  //         hmap::Array bedrock = z;
  //         hmap::remap(bedrock, z.min(), 1.1f * z.max());
  //         hmap::thermal(z, talus, iterations, &bedrock);
  //       },
  //       [&talus, &iterations](hmap::Array &z)
  //       {
  //         hmap::Array bedrock = z;
  //         hmap::remap(bedrock, z.min(), 1.1f * z.max());
  //         hmap::gpu::thermal(z, talus, iterations, &bedrock);
  //       },
  //       1e-3f,
  //       "diff_thermal_bedrock.png");
  // }

  // {
  //   hmap::Array talus(shape, 1.f / shape.x);
  //   int         iterations = 100;
  //   compare([&talus, &iterations](hmap::Array &z)
  //           { hmap::thermal_auto_bedrock(z, talus, iterations); },
  //           [&talus, &iterations](hmap::Array &z)
  //           { hmap::gpu::thermal_auto_bedrock(z, talus, iterations); },
  //           1e-3f,
  //           "diff_thermal_auto_bedrock.png");
  // }

  // {
  //   int ir = 64;
  //   compare([&ir](hmap::Array &z) { hmap::expand(z, ir); },
  //           [&ir](hmap::Array &z) { hmap::gpu::expand(z, ir); },
  //           1e-3f,
  //           "diff_expand.png");
  // }

  // {
  //   int ir = 64;
  //   compare([&ir](hmap::Array &z) { hmap::expand(z, ir, &z); },
  //           [&ir](hmap::Array &z) { hmap::gpu::expand(z, ir, &z); },
  //           1e-3f,
  //           "diff_expand_mask.png");
  // }

  // {
  //   int ir = 64;
  //   compare([&ir](hmap::Array &z) { hmap::shrink(z, ir); },
  //           [&ir](hmap::Array &z) { hmap::gpu::shrink(z, ir); },
  //           1e-3f,
  //           "diff_shrink.png");
  // }

  // {
  //   int ir = 64;
  //   compare(
  //       [&ir](hmap::Array &z)
  //       {
  //         hmap::Array mask = z;
  //         hmap::shrink(z, ir, &mask);
  //       },
  //       [&ir](hmap::Array &z)
  //       {
  //         hmap::Array mask = z;
  //         hmap::gpu::shrink(z, ir, &mask);
  //       },
  //       1e-3f,
  //       "diff_shrink_mask.png");
  // }

  // {
  //   int ir = 64;
  //   compare([&ir](hmap::Array &z) { hmap::smooth_cpulse(z, ir); },
  //           [&ir](hmap::Array &z) { hmap::gpu::smooth_cpulse(z, ir); },
  //           1e-3f,
  //           "diff_smooth_cpulse.png");
  // }

  // {
  //   int ir = 64;
  //   compare([&ir](hmap::Array &z) { hmap::smooth_cpulse(z, ir, &z); },
  //           [&ir](hmap::Array &z) { hmap::gpu::smooth_cpulse(z, ir, &z); },
  //           1e-3f,
  //           "diff_smooth_cpulse_mask.png");
  // }

  // {
  //   hmap::Array dx = hmap::noise_fbm(hmap::NoiseType::PERLIN,
  //                                    shape,
  //                                    {4.f, 4.f},
  //                                    2);
  //   hmap::Array dy = hmap::noise_fbm(hmap::NoiseType::PERLIN,
  //                                    shape,
  //                                    {4.f, 4.f},
  //                                    3);

  //   compare([&dx, &dy](hmap::Array &z) { hmap::warp(z, &dx, &dy); },
  //           [&dx, &dy](hmap::Array &z) { hmap::gpu::warp(z, &dx, &dy); },
  //           1e-3f,
  //           "diff_warp.png");
  // }

  // {
  //   compare([](hmap::Array &z) { hmap::laplace(z, 0.2f, 10); },
  //           [](hmap::Array &z) { hmap::gpu::laplace(z, 0.2f, 10); },
  //           1e-3f,
  //           "diff_laplace.png");
  // }

  // {
  //   compare([](hmap::Array &z) { hmap::laplace(z, &z, 0.2f, 10); },
  //           [](hmap::Array &z) { hmap::gpu::laplace(z, &z, 0.2f, 10); },
  //           1e-3f,
  //           "diff_laplace_masked.png");
  // }

  // {
  //   hmap::Array zr = hmap::noise_fbm(hmap::NoiseType::PERLIN,
  //                                    shape,
  //                                    {4.f, 4.f},
  //                                    2);
  //   compare([&zr](hmap::Array &z) { z = hmap::maximum_smooth(z, zr, 0.5f); },
  //           [&zr](hmap::Array &z)
  //           { z = hmap::gpu::maximum_smooth(z, zr, 0.5f); },
  //           1e-3f,
  //           "diff_maximum_smooth.png");
  // }

  // {
  //   hmap::Array zr = hmap::noise_fbm(hmap::NoiseType::PERLIN,
  //                                    shape,
  //                                    {4.f, 4.f},
  //                                    2);
  //   compare([&zr](hmap::Array &z) { z = hmap::minimum_smooth(z, zr, 0.5f); },
  //           [&zr](hmap::Array &z)
  //           { z = hmap::gpu::minimum_smooth(z, zr, 0.5f); },
  //           1e-3f,
  //           "diff_minimum_smooth.png");
  // }

  // {
  //   int   ir = 64;
  //   float k = 0.01f;
  //   compare([&ir, &k](hmap::Array &z) { hmap::smooth_fill(z, ir, k); },
  //           [&ir, &k](hmap::Array &z) { hmap::gpu::smooth_fill(z, ir, k); },
  //           1e-3f,
  //           "diff_smooth_fill.png");
  // }

  // {
  //   int   ir = 8;
  //   float k = 0.01f;
  //   compare([&ir, &k](hmap::Array &z) { hmap::smooth_fill(z, ir, &z, k); },
  //           [&ir, &k](hmap::Array &z) { hmap::gpu::smooth_fill(z, ir, &z, k);
  //           }, 1e-3f, "diff_smooth_fill_masked.png");
  // }

  // {
  //   compare([](hmap::Array &z) { z = hmap::gradient_norm(z); },
  //           [](hmap::Array &z) { z = hmap::gpu::gradient_norm(z); },
  //           1e-3f,
  //           "diff_gradient_norm.png");
  // }

  // {
  //   int ir = 64;
  //   compare([&ir](hmap::Array &z) { hmap::maximum_local_disk(z, ir); },
  //           [&ir](hmap::Array &z) { hmap::gpu::maximum_local_disk(z, ir); },
  //           1e-3f,
  //           "diff_maximum_local_disk.png");
  // }

  // {
  //   int ir = 64;
  //   compare([&ir](hmap::Array &z) { hmap::maximum_local_disk(z, ir); },
  //           [&ir](hmap::Array &z) { hmap::gpu::maximum_local_disk(z, ir); },
  //           1e-3f,
  //           "diff_minimum_local_disk.png");
  // }

  // {
  //   int ir = 64;
  //   compare([&ir](hmap::Array &z) { z = hmap::rugosity(z, ir); },
  //           [&ir](hmap::Array &z) { z = hmap::gpu::rugosity(z, ir); },
  //           1e-3f,
  //           "diff_rugosity.png");
  // }

  {
    int   ir = 32;
    float amount = 5.f;
    compare([&ir, &amount](hmap::Array &z)
            { hmap::normal_displacement(z, amount, ir); },
            [&ir, &amount](hmap::Array &z)
            { hmap::gpu::normal_displacement(z, amount, ir); },
            1e-3f,
            "diff_normal_displacement.png");
  }

  // {
  //   int   ir = 32;
  //   float amount = 2.f;
  //   compare([&ir, &amount](hmap::Array &z)
  //           { hmap::normal_displacement(z, &z, amount, ir); },
  //           [&ir, &amount](hmap::Array &z)
  //           { hmap::gpu::normal_displacement(z, &z, amount, ir); },
  //           1e-3f,
  //           "diff_normal_displacement_masked.png");
  // }

  // clwrapper::KernelManager::get_instance().set_block_size(32);
  // z2 = z;
  // hmap::Timer::Start("full GPU32");
  // hmap::gpu::median_3x3(z2);
  // hmap::Timer::Stop("full GPU32");

#else
  std::cout << "OpenCL not activated\n";
#endif
}