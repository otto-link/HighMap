#include <iostream>
#include <time.h>

// #include <xsimd.hpp>

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#include "highmap/dbg.hpp"

int main(void)
{

  hmap::Timer timer = hmap::Timer();

  // const std::vector<int>   shape = {2048, 2048};
  // const std::vector<int>   shape = {1024, 1024};
  const std::vector<int>   shape = {512, 512};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 2;

  // seed = (int)time(NULL);

  std::cout << "seed: " << seed << std::endl;

  timer.start("fbm");
  hmap::Array z = hmap::fbm_perlin(shape, res, seed, 3);
  timer.stop("fbm");
  hmap::remap(z);

  // z = hmap::crater(shape, 64, 0.1f, 32);

  auto noise = 0.1f * hmap::fbm_perlin(shape, res, seed, 8, 0.f);

  timer.start("wtri");
  z = hmap::wave_triangular(shape, 4.f, 30.f, 0.8f, &noise);
  timer.stop("wtri");

  // set_borders(z, 0.f, 256);

  z.infos();

  // z = hmap::maximum_smooth(z, hmap::crater(shape, 64, 0.3f, 24, 0.75f, {0.1f,
  // 0.1f}), 0.1f);

  // hmap::warp_fbm(z, 16.f, {4.f, 4.f}, seed);

  // float talus = 4.f / shape[0];

  // timer.start("inv 2");
  // auto c = hmap::select_gradient_exp(z, talus, 1.f / shape[0]);
  // timer.stop("inv 2");

  // auto weight = c; // hmap::constant(shape, 0.5f);
  // weight *= z;
  // hmap::remap(weight, 1.f, 0.f);

  // timer.start("fbm a");
  // hmap::Array za = hmap::fbm_perlin_advanced(shape, res, seed, 12, weight);
  // // za = c;

  // timer.stop("fbm a");

  // hmap::remap(za);
  // // hmap::recast_peak(za, 32, 1.f);
  // hmap::recurve_smoothstep_rational(za, 0.5f);

  // // z = hmap::fbm_perlin(shape, res, seed, 12, 1.f);
  // // hmap::smooth_fill_holes(z, 32);

  // // hmap::hydraulic_vpipes(za);

  // // hmap::remap(z);
  // // hmap::remap(za);
  auto z0 = z;

  // // hmap::laplace_edge_preserving(z, 2.f / shape[0], 0.2f, 10);

  // z.infos();
  // za.infos();

  // // hmap::recurve_s(za);

  // // // hmap::smooth_fill(z, 32);
  // // // hmap::warp_fbm(z, 16.f, {4.f, 4.f}, seed);

  // // if (false)
  // // {
  // //   int ir = 32;

  // //   auto zf = z;
  // //   hmap::smooth_cpulse(zf, ir);

  // //   auto  dx = hmap::Array(shape);
  // //   auto  dy = hmap::Array(shape);
  // //   auto  da = hmap::gradient_angle(z);
  // //   float alpha = 0.f / 180.f * 3.14f;

  // //   for (int i = 0; i < z.shape[0]; i++)
  // //     for (int j = 0; j < z.shape[1]; j++)
  // //     {
  // //       float c = std::abs(std::cos(alpha - da(i, j)));
  // //       dx(i, j) = -c * zf(i, j) * std::cos(alpha);
  // //       dy(i, j) = -c * zf(i, j) * std::sin(alpha);
  // //     }

  // //   smooth_cpulse(dx, ir);
  // //   smooth_cpulse(dy, ir);

  // //   dx *= 64.f;
  // //   dy *= 64.f;

  // //   dx.infos();

  // //   hmap::warp(z, dx, dy);
  // // }

  // // hmap::remap(z);
  // // // hmap::recurve(z, {0.f, 0.5f, 1.f}, {0.f, 0.2f, 1.f});

  // // // timer.start("fill");
  // // // hmap::smooth_fill_holes(z, 4);
  // // // timer.stop("fill");

  // // z.infos();

  z.to_png("out.png", hmap::cmap::terrain, true);
  z0.to_png("out0.png", hmap::cmap::terrain, true);
  z.to_file("out.bin");
}
