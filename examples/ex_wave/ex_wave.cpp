#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  float kw = 4.f;
  float angle = 30.f;
  uint  seed = 1;

  auto noise = 0.1f * hmap::fbm_perlin(shape, {kw, kw}, seed, 8, 0.f);

  auto zt = hmap::wave_triangular(shape, kw, angle, 0.8f);
  auto zq = hmap::wave_square(shape, kw, angle);
  auto zs = hmap::wave_sine(shape, kw, angle);

  auto ztn = hmap::wave_triangular(shape, kw, angle, 0.8f, &noise);
  auto zqn = hmap::wave_square(shape, kw, angle, &noise);
  auto zsn = hmap::wave_sine(shape, kw, angle, &noise);

  hmap::export_banner_png("ex_wave0.png", {zt, zq, zs}, hmap::cmap::viridis);

  hmap::export_banner_png("ex_wave1.png", {ztn, zqn, zsn}, hmap::cmap::viridis);
}
