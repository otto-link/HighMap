#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  // --- base

  hmap::Array z = hmap::gpu::gabor_wave(shape, kw, seed);
  hmap::Array z_fbm = hmap::gpu::gabor_wave_fbm(shape, kw, seed);

  // --- angle control

  float       angle = 45.f;
  float       angle_spread_ratio = 0.f;
  hmap::Array za0 = hmap::gpu::gabor_wave(shape,
                                          {16.f, 16.f},
                                          seed,
                                          angle,
                                          angle_spread_ratio);
  hmap::Array za1 = hmap::gpu::gabor_wave(shape, {16.f, 16.f}, seed, 0.f, 0.5f);

  hmap::Array za2 = hmap::gpu::gabor_wave_fbm(shape,
                                              {16.f, 16.f},
                                              seed,
                                              0.f,
                                              0.1f);

  // --- local angle

  hmap::Array field = hmap::noise(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::Array array_angle = hmap::gradient_angle(field) * 180.f / 3.14159f;

  angle_spread_ratio = 0.f;
  hmap::Array zr1 = hmap::gpu::gabor_wave(shape,
                                          {16.f, 16.f},
                                          seed,
                                          array_angle,
                                          angle_spread_ratio);

  hmap::Array zr2 = hmap::gpu::gabor_wave_fbm(shape,
                                              {16.f, 16.f},
                                              seed,
                                              array_angle,
                                              angle_spread_ratio);

  hmap::export_banner_png("ex_gabor_wave.png",
                          {z, z_fbm, za0, za1, za2, zr1, zr2},
                          hmap::Cmap::JET,
                          true);
}
