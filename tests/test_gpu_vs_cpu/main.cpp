/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <fstream>
#include <iostream>

#include "highmap.hpp"
#include "highmap/dbg/assert.hpp"

const int block_size = 32;
// const hmap::Vec2<int>   shape = {1024, 1024};
const hmap::Vec2<int>   shape = {256, 512};
const hmap::Vec2<float> kw = {2.f, 4.f};
const int               seed = 1;
std::fstream            f;

#ifdef ENABLE_OPENCL

template <typename F1, typename F2>
void compare(F1 fct1, F2 fct2, float tolerance, const std::string &name)
{
  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::remap(z);

  hmap::Array z1 = z;
  hmap::Array z2 = z;

  // host
  hmap::Timer::Start(name + " - host");
  fct1(z1);
  hmap::Timer::Stop(name + " - host");

  // GPU
  hmap::Timer::Start(name + " - GPU");
  fct2(z2);
  hmap::Timer::Stop(name + " - GPU");

  // retrieve timer data
  auto records = hmap::Timer::get_instance().get_records();

  hmap::AssertResults res;
  hmap::assert_almost_equal(z1, z2, tolerance, "diff_" + name + ".png", &res);
  res.msg += "[" + name + "]";
  res.print();

  float dt_host = records[name + " - host"]->total;
  float dt_gpu = records[name + " - GPU"]->total;

  f << name << ";";
  f << dt_host / dt_gpu << ";";
  f << dt_host << ";";
  f << dt_gpu << ";";
  f << (res.ret ? "ok" : "NOK") << ";";
  f << std::to_string(res.diff) << ";";
  f << std::to_string(res.tolerance) << ";";
  f << std::to_string(res.count) << ";";
  f << res.msg << ";";
  f << "\n";
}

#endif

// ---

int main(void)
{

#ifdef ENABLE_OPENCL
  hmap::gpu::init_opencl();

  clwrapper::KernelManager::get_instance().set_block_size(block_size);

  f.open("test_gpu_vs_cpu.csv", std::ios::out);

  f << "#name" << ";";
  f << "speedup [-]" << ";";
  f << "CPU [ms]" << ";";
  f << "GPU [ms]" << ";";
  f << "ok / NOK" << ";";
  f << "diff" << ";";
  f << "tolerance" << ";";
  f << "count" << ";";
  f << "msg" << ";";
  f << "\n";

  int ir = 64;

  compare([&ir](hmap::Array &z) { hmap::expand(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::expand(z, ir); },
          1e-3f,
          "expand");

  compare([&ir](hmap::Array &z) { hmap::expand(z, ir, &z); },
          [&ir](hmap::Array &z) { hmap::gpu::expand(z, ir, &z); },
          1e-3f,
          "expand_mask");

  compare([](hmap::Array &z) { z = hmap::gradient_norm(z); },
          [](hmap::Array &z) { z = hmap::gpu::gradient_norm(z); },
          1e-3f,
          "gradient_norm");

  {
    int nparticles = 50000;
    compare([&nparticles](hmap::Array &z)
            { hmap::hydraulic_particle(z, nparticles, 0); },
            [&nparticles](hmap::Array &z)
            { hmap::gpu::hydraulic_particle(z, &z, nparticles, 0); },
            1e-3f,
            "hydraulic_particle");
  }

  compare([](hmap::Array &z) { hmap::laplace(z, 0.2f, 10); },
          [](hmap::Array &z) { hmap::gpu::laplace(z, 0.2f, 10); },
          1e-3f,
          "laplace");

  compare([](hmap::Array &z) { hmap::laplace(z, &z, 0.2f, 10); },
          [](hmap::Array &z) { hmap::gpu::laplace(z, &z, 0.2f, 10); },
          1e-3f,
          "laplace_masked");

  {
    hmap::Array zr = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                     shape,
                                     {4.f, 4.f},
                                     2);
    compare([&zr](hmap::Array &z) { z = hmap::maximum_smooth(z, zr, 0.5f); },
            [&zr](hmap::Array &z)
            { z = hmap::gpu::maximum_smooth(z, zr, 0.5f); },
            1e-3f,
            "maximum_smooth");
  }

  compare([&ir](hmap::Array &z) { hmap::maximum_local_disk(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::maximum_local_disk(z, ir); },
          1e-3f,
          "maximum_local_disk");

  compare([](hmap::Array &z) { hmap::median_3x3(z); },
          [](hmap::Array &z) { hmap::gpu::median_3x3(z); },
          1e-3f,
          "median_3x3");

  {
    hmap::Array zr = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                     shape,
                                     {4.f, 4.f},
                                     2);
    compare([&zr](hmap::Array &z) { z = hmap::minimum_smooth(z, zr, 0.5f); },
            [&zr](hmap::Array &z)
            { z = hmap::gpu::minimum_smooth(z, zr, 0.5f); },
            1e-3f,
            "minimum_smooth");
  }

  compare([&ir](hmap::Array &z) { z = hmap::maximum_local(z, ir); },
          [&ir](hmap::Array &z) { z = hmap::gpu::maximum_local(z, ir); },
          1e-3f,
          "maximum_local");

  compare([&ir](hmap::Array &z) { z = hmap::minimum_local(z, ir); },
          [&ir](hmap::Array &z) { z = hmap::gpu::minimum_local(z, ir); },
          1e-3f,
          "minimum_local");

  compare([&ir](hmap::Array &z) { hmap::maximum_local_disk(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::maximum_local_disk(z, ir); },
          1e-3f,
          "minimum_local_disk");

  {
    int   ir = 32;
    float amount = 5.f;
    compare([&ir, &amount](hmap::Array &z)
            { hmap::normal_displacement(z, amount, ir); },
            [&ir, &amount](hmap::Array &z)
            { hmap::gpu::normal_displacement(z, amount, ir); },
            1e-3f,
            "normal_displacement");
  }

  {
    int   ir = 32;
    float amount = 2.f;
    compare([&ir, &amount](hmap::Array &z)
            { hmap::normal_displacement(z, &z, amount, ir); },
            [&ir, &amount](hmap::Array &z)
            { hmap::gpu::normal_displacement(z, &z, amount, ir); },
            1e-3f,
            "normal_displacement_mask");
  }

  compare([&ir](hmap::Array &z) { hmap::plateau(z, ir, 4.f); },
          [&ir](hmap::Array &z) { hmap::gpu::plateau(z, ir, 4.f); },
          1e-3f,
          "plateau");

  compare([&ir](hmap::Array &z) { hmap::plateau(z, &z, ir, 4.f); },
          [&ir](hmap::Array &z) { hmap::gpu::plateau(z, &z, ir, 4.f); },
          1e-3f,
          "plateau_mask");

  compare([&ir](hmap::Array &z) { z = hmap::relative_elevation(z, ir); },
          [&ir](hmap::Array &z) { z = hmap::gpu::relative_elevation(z, ir); },
          1e-3f,
          "relative_elevation");

  compare([&ir](hmap::Array &z) { z = hmap::rugosity(z, ir); },
          [&ir](hmap::Array &z) { z = hmap::gpu::rugosity(z, ir); },
          1e-3f,
          "rugosity");

  compare([&ir](hmap::Array &z) { hmap::shrink(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::shrink(z, ir); },
          1e-3f,
          "shrink");

  compare(
      [&ir](hmap::Array &z)
      {
        hmap::Array mask = z;
        hmap::shrink(z, ir, &mask);
      },
      [&ir](hmap::Array &z)
      {
        hmap::Array mask = z;
        hmap::gpu::shrink(z, ir, &mask);
      },
      1e-3f,
      "shrink_mask");

  {
    hmap::Array base = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                       shape,
                                       {2.f, 8.f},
                                       0);
    hmap::remap(base, -0.5f, 0.4f);
    hmap::make_binary(base);

    compare([&base](hmap::Array &z) { z = hmap::skeleton(base); },
            [&base](hmap::Array &z) { z = hmap::gpu::skeleton(base); },
            1e-3f,
            "skeleton");
  }

  compare([&ir](hmap::Array &z) { hmap::smooth_cpulse(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::smooth_cpulse(z, ir); },
          1e-3f,
          "smooth_cpulse");

  compare([&ir](hmap::Array &z) { hmap::smooth_cpulse(z, ir, &z); },
          [&ir](hmap::Array &z) { hmap::gpu::smooth_cpulse(z, ir, &z); },
          1e-3f,
          "smooth_cpulse_mask");

  compare([&ir](hmap::Array &z) { hmap::smooth_fill(z, ir, 0.01f); },
          [&ir](hmap::Array &z) { hmap::gpu::smooth_fill(z, ir, 0.01f); },
          1e-3f,
          "smooth_fill");

  compare([&ir](hmap::Array &z) { hmap::smooth_fill(z, ir, &z, 0.01f); },
          [&ir](hmap::Array &z) { hmap::gpu::smooth_fill(z, ir, &z, 0.01f); },
          1e-3f,
          "smooth_fill_mask");

  {
    hmap::Array talus(shape, 0.5f / shape.x);
    int         iterations = 100;
    compare([&talus, &iterations](hmap::Array &z)
            { hmap::thermal(z, talus, iterations); },
            [&talus, &iterations](hmap::Array &z)
            { hmap::gpu::thermal(z, talus, iterations); },
            1e-3f,
            "thermal");
  }

  {
    hmap::Array talus(shape, 2.f / shape.x);
    int         iterations = 100;
    compare(
        [&talus, &iterations](hmap::Array &z)
        {
          hmap::Array bedrock = z;
          hmap::remap(bedrock, z.min(), 1.1f * z.max());
          hmap::thermal(z, talus, iterations, &bedrock);
        },
        [&talus, &iterations](hmap::Array &z)
        {
          hmap::Array bedrock = z;
          hmap::remap(bedrock, z.min(), 1.1f * z.max());
          hmap::gpu::thermal(z, talus, iterations, &bedrock);
        },
        1e-3f,
        "thermal_bedrock");
  }

  {
    hmap::Array talus(shape, 1.f / shape.x);
    int         iterations = 100;
    compare([&talus, &iterations](hmap::Array &z)
            { hmap::thermal_auto_bedrock(z, talus, iterations); },
            [&talus, &iterations](hmap::Array &z)
            { hmap::gpu::thermal_auto_bedrock(z, talus, iterations); },
            1e-3f,
            "thermal_auto_bedrock");
  }

  compare([](hmap::Array &z) { hmap::thermal_rib(z, 10); },
          [](hmap::Array &z) { hmap::gpu::thermal_rib(z, 10); },
          1e-3f,
          "thermal_rib");

  {
    hmap::Array dx = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                     shape,
                                     {4.f, 4.f},
                                     2);
    hmap::Array dy = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                     shape,
                                     {4.f, 4.f},
                                     3);

    compare([&dx, &dy](hmap::Array &z) { hmap::warp(z, &dx, &dy); },
            [&dx, &dy](hmap::Array &z) { hmap::gpu::warp(z, &dx, &dy); },
            1e-3f,
            "warp");
  }

  f.close();

#else
  std::cout << "OpenCL not activated\n";
#endif
}