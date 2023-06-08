#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  float                  res = 4.f;
  float                  angle = 30.f;

  auto zt = hmap::wave_triangular(shape, res, angle, 0.8f);

  // add some warping
  auto zt_warp = zt;
  hmap::warp_fbm(zt_warp, 16, {4.f, 4.f}, 1);

  auto zq = hmap::wave_square(shape, res, angle);
  auto zs = hmap::wave_sine(shape, res, angle);

  zt.to_file("out.bin");

  hmap::export_banner_png("ex_wave.png",
                          {zt, zt_warp, zq, zs},
                          hmap::cmap::viridis);
}
