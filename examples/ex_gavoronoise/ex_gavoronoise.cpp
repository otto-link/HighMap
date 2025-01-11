#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  // --- base usage

  hmap::Array z1 = hmap::gpu::gavoronoise(shape, kw, seed);

  float       amp = 0.05f;
  float       angle = 45.f;
  float       angle_spread_ratio = 0.f;
  hmap::Array z2 = hmap::gpu::gavoronoise(shape,
                                          kw,
                                          seed,
                                          angle,
                                          amp,
                                          angle_spread_ratio);

  // --- with input base noise

  int         octaves = 2;
  hmap::Array base = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                     shape,
                                     kw,
                                     ++seed,
                                     octaves);

  // base amplitude amplitude expected to be in [-1, 1] (approx.)
  hmap::remap(base, -1.f, 1.f);
  hmap::Array z3 = hmap::gpu::gavoronoise(base, kw, seed, amp);

  // --- local angle

  hmap::Array field = hmap::noise(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::Array array_angle = hmap::gradient_angle(field) * 180.f / 3.14159f;

  angle_spread_ratio = 0.f;
  hmap::Array z4 = hmap::gpu::gavoronoise(shape,
                                          {8.f, 8.f},
                                          seed,
                                          array_angle,
                                          amp,
                                          angle_spread_ratio);

  hmap::export_banner_png("ex_gavoronoise.png",
                          {z1, z2, z3, z4},
                          hmap::Cmap::JET,
                          true);
}
