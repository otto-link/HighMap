#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 1;

  hmap::Array z = hmap::white(shape, 0.f, 1.f, seed);
  hmap::Array kernel = hmap::disk({32, 32});

  kernel.normalize();

  // SVD approximation
  int  rank = 4;
  auto zs = hmap::convolve2d_svd(z, kernel, rank);

  // exact convolution
  auto zc = hmap::convolve2d(z, kernel);

  remap(zs);
  remap(zc);

  hmap::export_banner_png("ex_convolve2d_svd.png",
                          {z, zs, zc},
                          hmap::Cmap::VIRIDIS);
}
