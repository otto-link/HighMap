#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/math.hpp"
#include "highmap/noise.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::white(shape, 0.f, 1.f, seed);
  hmap::Array kernel = hmap::disk({32, 32});

  kernel.normalize();

  // SVD approximation
  int  rank = 4;
  auto zs = hmap::convolve2d_svd(z, kernel, rank);

  // exact convolution
  auto zc = hmap::convolve2d(z, kernel);

  z.to_png("ex_convolve2d_svd0.png", hmap::cmap::gray);
  zs.to_png("ex_convolve2d_svd1.png", hmap::cmap::gray);
  zc.to_png("ex_convolve2d_svd2.png", hmap::cmap::gray);
}
