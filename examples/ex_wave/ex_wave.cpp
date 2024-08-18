#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  float kw = 4.f;
  float angle = 30.f;
  uint  seed = 1;

  auto noise = 0.1f * hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                      shape,
                                      {kw, kw},
                                      seed,
                                      8,
                                      0.f);

  auto zt = hmap::wave_triangular(shape, kw, angle, 0.8f);
  auto zq = hmap::wave_square(shape, kw, angle);
  auto zs = hmap::wave_sine(shape, kw, angle);
  auto zd = hmap::wave_dune(shape, kw, angle, 0.5f, 0.7f, 0.f);

  auto ztn = hmap::wave_triangular(shape, kw, angle, 0.8f, 0.f, &noise);
  auto zqn = hmap::wave_square(shape, kw, angle, 0.f, &noise);
  auto zsn = hmap::wave_sine(shape, kw, angle, 0.f, &noise);
  auto zdn = hmap::wave_dune(shape, kw, angle, 0.5f, 0.7f, 0.f, &noise);

  hmap::export_banner_png("ex_wave0.png",
                          {zt, zq, zs, zd},
                          hmap::Cmap::VIRIDIS);
  hmap::export_banner_png("ex_wave1.png",
                          {ztn, zqn, zsn, zdn},
                          hmap::Cmap::VIRIDIS);
}
