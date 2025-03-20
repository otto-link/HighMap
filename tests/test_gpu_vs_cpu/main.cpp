/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <fstream>
#include <iostream>

#include "highmap.hpp"
#include "highmap/dbg/assert.hpp"
#include "highmap/dbg/timer.hpp"

// const hmap::Vec2<int>   shape = {2048, 2048};
// const hmap::Vec2<int> shape = {1024, 1024};
const hmap::Vec2<int>   shape = {256, 512};
const hmap::Vec2<float> kw = {2.f, 4.f};
const int               seed = 1;
std::fstream            f;

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

// ---

int main(void)
{

  hmap::gpu::init_opencl();

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

  compare([&ir](hmap::Array &z) { hmap::accumulation_curvature(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::accumulation_curvature(z, ir); },
          1e-3f,
          "accumulation_curvature");

  compare([ir](hmap::Array &z) { z = hmap::border(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::border(z, ir); },
          1e-3f,
          "border");

  compare([ir](hmap::Array &z) { z = hmap::closing(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::closing(z, ir); },
          1e-3f,
          "closing");

  compare([&ir](hmap::Array &z) { hmap::accumulation_curvature(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::accumulation_curvature(z, ir); },
          1e-3f,
          "accumulation_curvature");

  compare([&ir](hmap::Array &z)
          { hmap::curvature_horizontal_cross_sectional(z, ir); },
          [&ir](hmap::Array &z)
          { hmap::gpu::curvature_horizontal_cross_sectional(z, ir); },
          1e-3f,
          "curvature_horizontal_cross_sectional");

  compare([&ir](hmap::Array &z) { hmap::curvature_horizontal_plan(z, ir); },
          [&ir](hmap::Array &z)
          { hmap::gpu::curvature_horizontal_plan(z, ir); },
          1e-3f,
          "curvature_horizontal_plan");

  compare([&ir](hmap::Array &z)
          { hmap::curvature_horizontal_tangential(z, ir); },
          [&ir](hmap::Array &z)
          { hmap::gpu::curvature_horizontal_tangential(z, ir); },
          1e-3f,
          "curvature_horizontal_tangential");

  compare([&ir](hmap::Array &z) { hmap::curvature_ring(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::curvature_ring(z, ir); },
          1e-3f,
          "curvature_ring");

  compare([&ir](hmap::Array &z) { hmap::curvature_rotor(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::curvature_rotor(z, ir); },
          1e-3f,
          "curvature_rotor");

  compare([&ir](hmap::Array &z)
          { hmap::curvature_vertical_longitudinal(z, ir); },
          [&ir](hmap::Array &z)
          { hmap::gpu::curvature_vertical_longitudinal(z, ir); },
          1e-3f,
          "curvature_vertical_longitudinal");

  compare([&ir](hmap::Array &z) { hmap::curvature_vertical_profile(z, ir); },
          [&ir](hmap::Array &z)
          { hmap::gpu::curvature_vertical_profile(z, ir); },
          1e-3f,
          "curvature_vertical_profile");

  compare([ir](hmap::Array &z) { z = hmap::dilation(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::dilation(z, ir); },
          1e-3f,
          "dilation");

  compare([ir](hmap::Array &z) { z = hmap::erosion(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::erosion(z, ir); },
          1e-3f,
          "erosion");

  compare([&ir](hmap::Array &z) { hmap::expand(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::expand(z, ir); },
          1e-3f,
          "expand");

  compare([&ir](hmap::Array &z) { hmap::expand(z, ir, &z); },
          [&ir](hmap::Array &z) { hmap::gpu::expand(z, ir, &z); },
          1e-3f,
          "expand_mask");

  compare([](hmap::Array &z) { z = hmap::flow_direction_d8(z); },
          [](hmap::Array &z) { z = hmap::gpu::flow_direction_d8(z); },
          1e-3f,
          "flow_direction_d8");

  compare([&ir](hmap::Array &z) { hmap::gamma_correction_local(z, 0.5f, ir); },
          [&ir](hmap::Array &z)
          { hmap::gpu::gamma_correction_local(z, 0.5f, ir); },
          1e-3f,
          "gamma_correction_local");

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

  compare([ir](hmap::Array &z)
          { hmap::hydraulic_stream_log(z, 0.1f, 5.f / 512.f, 64); },
          [ir](hmap::Array &z)
          { hmap::gpu::hydraulic_stream_log(z, 0.1f, 5.f / 512.f, 64); },
          1e-3f,
          "hydraulic_stream_log");

  compare([](hmap::Array &z) { hmap::laplace(z, 0.2f, 10); },
          [](hmap::Array &z) { hmap::gpu::laplace(z, 0.2f, 10); },
          1e-3f,
          "laplace");

  compare([](hmap::Array &z) { hmap::laplace(z, &z, 0.2f, 10); },
          [](hmap::Array &z) { hmap::gpu::laplace(z, &z, 0.2f, 10); },
          1e-3f,
          "laplace_masked");

  compare([&ir](hmap::Array &z) { z = hmap::maximum_local(z, ir); },
          [&ir](hmap::Array &z) { z = hmap::gpu::maximum_local(z, ir); },
          1e-3f,
          "maximum_local");

  compare([&ir](hmap::Array &z) { hmap::maximum_local_disk(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::maximum_local_disk(z, ir); },
          1e-3f,
          "maximum_local_disk");

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

  compare([ir](hmap::Array &z) { z = hmap::mean_local(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::mean_local(z, ir); },
          1e-3f,
          "mean_local");

  compare([ir](hmap::Array &z)
          { z = hmap::mean_shift(z, ir, 16.f / z.shape.x, 4); },
          [ir](hmap::Array &z)
          { z = hmap::gpu::mean_shift(z, ir, 16.f / z.shape.x, 4); },
          1e-3f,
          "mean_shift");

  compare([](hmap::Array &z) { hmap::median_3x3(z); },
          [](hmap::Array &z) { hmap::gpu::median_3x3(z); },
          1e-3f,
          "median_3x3");

  compare([&ir](hmap::Array &z) { z = hmap::minimum_local(z, ir); },
          [&ir](hmap::Array &z) { z = hmap::gpu::minimum_local(z, ir); },
          1e-3f,
          "minimum_local");

  compare([&ir](hmap::Array &z) { hmap::maximum_local_disk(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::maximum_local_disk(z, ir); },
          1e-3f,
          "minimum_local_disk");

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

  compare([ir](hmap::Array &z) { z = hmap::morphological_black_hat(z, ir); },
          [ir](hmap::Array &z)
          { z = hmap::gpu::morphological_black_hat(z, ir); },
          1e-3f,
          "morphological_black_hat");

  compare([ir](hmap::Array &z) { z = hmap::morphological_gradient(z, ir); },
          [ir](hmap::Array &z)
          { z = hmap::gpu::morphological_gradient(z, ir); },
          1e-3f,
          "morphological_gradient");

  compare([ir](hmap::Array &z) { z = hmap::morphological_top_hat(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::morphological_top_hat(z, ir); },
          1e-3f,
          "morphological_top_hat");

  {

    std::vector<hmap::NoiseType> types = {
        // hmap::NoiseType::PARBERRY,
        hmap::NoiseType::PERLIN,
        hmap::NoiseType::PERLIN_BILLOW,
        hmap::NoiseType::PERLIN_HALF,
        hmap::NoiseType::SIMPLEX2,
        // hmap::NoiseType::SIMPLEX2S,
        hmap::NoiseType::VALUE,
        hmap::NoiseType::VALUE_CUBIC,
        // hmap::NoiseType::VALUE_DELAUNAY,
        hmap::NoiseType::VALUE_LINEAR,
        hmap::NoiseType::WORLEY
        // hmap::NoiseType::WORLEY_DOUBLE,
        // hmap::NoiseType::WORLEY_VALUE
    };

    hmap::Vec2<float> kw = {32.f, 32.f};

    for (auto type : types)
    {
      compare([type, kw](hmap::Array &z)
              { z = hmap::noise(type, shape, kw, 1); },
              [type, kw](hmap::Array &z)
              { z = hmap::gpu::noise(type, shape, kw, 1); },
              1e-3f,
              "noise" + std::to_string(type));
    }

    // fbm
    kw = {4.f, 4.f};
    for (auto type : types)
    {
      compare(
          [type, kw](hmap::Array &z)
          {
            if (type != hmap::NoiseType::VALUE_LINEAR)
              z = hmap::noise_fbm(type, shape, kw, 1);
            else
              z = 0.f;
          },
          [type, kw](hmap::Array &z)
          { z = hmap::gpu::noise_fbm(type, shape, kw, 1); },
          1e-3f,
          "noise_fbm" + std::to_string(type));
    }
  }

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

  compare([ir](hmap::Array &z) { z = hmap::opening(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::opening(z, ir); },
          1e-3f,
          "opening");

  compare([&ir](hmap::Array &z) { hmap::plateau(z, ir, 4.f); },
          [&ir](hmap::Array &z) { hmap::gpu::plateau(z, ir, 4.f); },
          1e-3f,
          "plateau");

  compare([&ir](hmap::Array &z) { hmap::plateau(z, &z, ir, 4.f); },
          [&ir](hmap::Array &z) { hmap::gpu::plateau(z, &z, ir, 4.f); },
          1e-3f,
          "plateau_mask");

  {
    hmap::Array base = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                       shape,
                                       {2.f, 8.f},
                                       0);
    hmap::remap(base, -0.5f, 0.4f);
    hmap::make_binary(base);

    int ir = 32;

    compare([&base, &ir](hmap::Array &z)
            { z = hmap::relative_distance_from_skeleton(base, ir); },
            [&base, &ir](hmap::Array &z)
            { z = hmap::gpu::relative_distance_from_skeleton(base, ir); },
            1e-3f,
            "relative_distance_from_skeleton");
  }

  compare([&ir](hmap::Array &z) { z = hmap::relative_elevation(z, ir); },
          [&ir](hmap::Array &z) { z = hmap::gpu::relative_elevation(z, ir); },
          1e-3f,
          "relative_elevation");

  compare([&ir](hmap::Array &z) { z = hmap::ruggedness(z, ir); },
          [&ir](hmap::Array &z) { z = hmap::gpu::ruggedness(z, ir); },
          1e-3f,
          "ruggedness");

  compare([&ir](hmap::Array &z) { z = hmap::rugosity(z, ir); },
          [&ir](hmap::Array &z) { z = hmap::gpu::rugosity(z, ir); },
          1e-3f,
          "rugosity");

  compare([&ir](hmap::Array &z) { hmap::shrink(z, ir); },
          [&ir](hmap::Array &z) { hmap::gpu::shrink(z, ir); },
          1e-3f,
          "shrink");

  {
    hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
    hmap::Path path = hmap::Path(200, 0, bbox.adjust(0.2f, -0.2f, 0.2f, -0.2f));
    path.reorder_nns();

    compare([bbox, path](hmap::Array &z)
            { z = hmap::sdf_2d_polyline(path, z.shape, bbox); },
            [bbox, path](hmap::Array &z)
            { z = hmap::gpu::sdf_2d_polyline(path, z.shape, bbox); },
            1e-3f,
            "sdf_2d_polyline");
  }

  {
    hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
    hmap::Path path = hmap::Path(200, 0, bbox.adjust(0.2f, -0.2f, 0.2f, -0.2f));
    path.reorder_nns();

    compare([bbox, path](hmap::Array &z)
            { z = hmap::sdf_2d_polyline_bezier(path, z.shape, bbox); },
            [bbox, path](hmap::Array &z)
            { z = hmap::gpu::sdf_2d_polyline_bezier(path, z.shape, bbox); },
            1e-3f,
            "sdf_2d_polyline_bezier");
  }

  compare([ir](hmap::Array &z) { z = hmap::shape_index(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::shape_index(z, ir); },
          1e-3f,
          "shape_index");

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

  compare([ir](hmap::Array &z) { hmap::smooth_fill_smear_peaks(z, ir); },
          [ir](hmap::Array &z) { hmap::gpu::smooth_fill_smear_peaks(z, ir); },
          1e-3f,
          "smooth_fill_smear_peaks");

  compare([ir](hmap::Array &z) { hmap::smooth_fill_holes(z, ir); },
          [ir](hmap::Array &z) { hmap::gpu::smooth_fill_holes(z, ir); },
          1e-3f,
          "smooth_fill_holes");

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

  compare([ir](hmap::Array &z) { z = hmap::unsphericity(z, ir); },
          [ir](hmap::Array &z) { z = hmap::gpu::unsphericity(z, ir); },
          1e-3f,
          "unsphericity");

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
}
