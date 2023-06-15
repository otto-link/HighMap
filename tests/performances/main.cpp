#include <iostream>

#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

#include "highmap/dbg.hpp"

int main(void)
{

  hmap::Timer timer = hmap::Timer();

  // const std::vector<int> shape = {4096, 4096};
  // const std::vector<int> shape = {2048, 2048};
  const std::vector<int> shape = {1024, 1024};
  // const std::vector<int> shape = {512, 512};
  // const std::vector<int> shape = {64, 64};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 2;

  hmap::Array z = hmap::Array(shape);

  for (int it = 0; it < 5; it++)
  {
    timer.start("fbm_perlin");
    z = hmap::fbm_perlin(shape, res, seed, 8);
    timer.stop("fbm_perlin");
  }

  auto  z0 = z;
  int   n = 5;
  float gamma = 1.f;

  {
    z = z0;
    for (int it = 0; it < n; it++)
    {
      timer.start("gamma_correction");
      hmap::gamma_correction(z, gamma);
      timer.stop("gamma_correction");
    }
    z.to_png("perf0.png", hmap::cmap::viridis);
  }

  {
    z = z0;
    for (int it = 0; it < n; it++)
    {
      timer.start("gamma_correction_xsimd");
      hmap::gamma_correction_xsimd(z, gamma);
      timer.stop("gamma_correction_xsimd");
    }
  }

  {
    z = z0;
    for (int it = 0; it < n; it++)
    {
      std::cout << "it #" << it << std::endl;
      timer.start("gamma_correction_thread");
      hmap::gamma_correction_thread(z, gamma);
      timer.stop("gamma_correction_thread");
    }
    z.to_png("perf1.png", hmap::cmap::viridis);
  }
}
