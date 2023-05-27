#include <iostream>
#include <time.h>

#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#include "highmap/dbg.hpp"

int main(void)
{
  hmap::Timer timer = hmap::Timer();

  // const std::vector<int>   shape = {1024, 1024};
  const std::vector<int>   shape = {512, 512};
  const std::vector<float> res = {3.f, 3.f};
  int                      seed = 3;

  seed = (int)time(NULL);

  std::cout << "seed: " << seed << std::endl;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::Array z1 = hmap::fbm_perlin(shape, res, seed);
  hmap::Array z2 = hmap::worley(shape, res, seed + 1);

  // z1 = z1 + 0.5f;
  // z2 = z2 + 1.f;

  remap(z1, 0.f, 0.7f);
  warp_fbm(z1, 32.f, {4.f, 4.f}, seed + 1);

  remap(z2, 0.2, 1.);
  gamma_correction(z2, 0.5);
  warp_fbm(z2, 64.f, {4.f, 4.f}, seed);
  z2 = z2 * hmap::biweight(shape);

  // low_pass_high_order(z2, 5);

  auto zm = maximum_smooth(z1, z2, 0.05f);

  z2 = zm * hmap::biweight(shape);

  // z2 = z2 + 0.1f * z1 * hmap::smooth_cosine(shape);

  remap(z2);
  // gamma_correction(z2, 2.f);

  // hmap::hydraulic_benes(z2, 10);

  z = z2;

  // warp_fbm(z2, 128.f, res, seed);
  // z2 = z2 * hmap::tricube(shape);

  // z1.infos();
  // z2.infos();

  // z = hmap::minimum_smooth(z1, z2, 0.01f);
  // // z = hmap::blend_overlay(z2, z1);

  remap(z, 0, 1);

  auto z0 = z;

  z = hmap::fbm_perlin(shape, res, seed);
  remap(z);
  z = z * hmap::biweight(shape) + 0.05f * z;

  // // z = z + hmap::smooth_cosine(shape);
  // z = z + 2.f * hmap::biweight(shape);

  // // auto dx = hmap::fbm_perlin(shape, res, seed + 1);
  // // auto dy = hmap::fbm_perlin(shape, res, seed + 2);

  // // float scale = 128.f;
  // // remap(dx, -scale, scale);
  // // remap(dy, -scale, scale);

  // // hmap::remap(z);
  // // dx = dx * z;
  // // dy = dy * z;
  // // warp(z, dx, dy);

  // // timer.start("worley");
  // // auto w = hmap::worley(shape, {1.f, 2.f}, seed);
  // // remap(w);
  // // z = z + 2.f * w;
  // // timer.stop("worley");

  // // hmap::set_borders(z, 0.f, 124);

  // // timer.start("stratify");
  // // int  ns = 10;
  // // auto hs = hmap::linspace_jitted(z.min(), z.max(), ns, 0.9f, seed);
  // // auto gs = hmap::random_vector(0.2, 0.8, ns - 1, seed);
  // // hmap::stratify(z, hs, gs);
  // // timer.stop("stratify");

  // hmap::remap(z);
  // auto z0 = z;

  // // timer.start("thermal");
  // // hmap::thermal(z, 0.1f * z.ptp() / shape[0], 100);
  // // timer.stop("thermal");

  hmap::Array z_bedrock = hmap::Array(shape);

  z0 = z;

  for (int k = 0; k < 4; k++)
  {
    timer.start("cycle");

    // timer.start("thermal");
    // hmap::thermal_auto_bedrock(z, 0.8f * z.ptp() / shape[0], 1);
    // timer.stop("thermal");

    timer.start("stream");
    z_bedrock = hmap::minimum_local(z, 11);
    hmap::hydraulic_stream(z, z_bedrock, 0.001f, 0.01f / shape[0]);
    timer.stop("stream");

    timer.start("hydraulic_particle");
    float nparticles = (int)(0.1f * z.shape[0] * z.shape[1]);
    hmap::hydraulic_particle(z, z, nparticles, seed, 0, 30., 0.2, 0.5, 0.01);
    timer.stop("hydraulic_particle");

    timer.stop("cycle");

    z.to_png("out.png", hmap::cmap::terrain, true);
  }

  // hmap::thermal_auto_bedrock(z, 0.1f * z.ptp() / shape[0], 5);

  // z = lerp(z0, z, 0.5f);

  // // z = hmap::blend_soft(z0, z);

  z.to_png("out.png", hmap::cmap::terrain, true);
  z.to_file("out.bin");
}
