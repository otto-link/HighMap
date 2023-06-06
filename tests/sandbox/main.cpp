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
  // const std::vector<int> shape = {1024, 1024};
  const std::vector<int>   shape = {512, 512};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 2;

  // seed = (int)time(NULL);

  std::cout << "seed: " << seed << std::endl;

  timer.start("fbm");
  hmap::Array z = hmap::fbm_perlin(shape, res, seed, 12);
  timer.stop("fbm");

  timer.start("fbm a");
  hmap::Array za = hmap::fbm_perlin_advanced(shape, res, seed, 12);
  timer.stop("fbm a");

  // hmap::remap(z);
  // hmap::remap(za);
  auto z0 = za;

  // hmap::laplace_edge_preserving(z, 2.f / shape[0], 0.2f, 10);

  z.infos();
  za.infos();

  // hmap::recurve_s(za);

  // // hmap::smooth_fill(z, 32);
  // // hmap::warp_fbm(z, 16.f, {4.f, 4.f}, seed);

  // if (false)
  // {
  //   int ir = 32;

  //   auto zf = z;
  //   hmap::smooth_cpulse(zf, ir);

  //   auto  dx = hmap::Array(shape);
  //   auto  dy = hmap::Array(shape);
  //   auto  da = hmap::gradient_angle(z);
  //   float alpha = 0.f / 180.f * 3.14f;

  //   for (int i = 0; i < z.shape[0]; i++)
  //     for (int j = 0; j < z.shape[1]; j++)
  //     {
  //       float c = std::abs(std::cos(alpha - da(i, j)));
  //       dx(i, j) = -c * zf(i, j) * std::cos(alpha);
  //       dy(i, j) = -c * zf(i, j) * std::sin(alpha);
  //     }

  //   smooth_cpulse(dx, ir);
  //   smooth_cpulse(dy, ir);

  //   dx *= 64.f;
  //   dy *= 64.f;

  //   dx.infos();

  //   hmap::warp(z, dx, dy);
  // }

  // hmap::remap(z);
  // // hmap::recurve(z, {0.f, 0.5f, 1.f}, {0.f, 0.2f, 1.f});

  // // timer.start("fill");
  // // hmap::smooth_fill_holes(z, 4);
  // // timer.stop("fill");

  // z.infos();

  z.to_png("out.png", hmap::cmap::terrain, true);
  z0.to_png("out0.png", hmap::cmap::terrain, true);
  z.to_file("out.bin");
}
