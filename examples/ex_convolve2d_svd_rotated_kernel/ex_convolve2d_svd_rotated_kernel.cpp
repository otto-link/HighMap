#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::perlin(shape, res, seed);
  hmap::remap(z);

  hmap::Array kernel = hmap::fbm_worley({64, 64}, res, seed);
  hmap::remap(kernel);
  hmap::set_borders(kernel, 0.f, 16);

  kernel.normalize();

<<<<<<< HEAD
  int   rank = 4;
  int   n_rotations = 6;
  float density = 0.1f;
  auto  zs = hmap::convolve2d_svd_rotated_kernel(z,
                                                kernel,
                                                rank,
                                                n_rotations,
                                                density);
=======
  int rank = 4;
  int n_rotations = 6;

  auto zs = hmap::convolve2d_svd_rotated_kernel(z, kernel, rank, n_rotations);
>>>>>>> dev
  hmap::remap(zs);

  hmap::export_banner_png("ex_convolve2d_svd_rotated_kernel.png",
                          {z, zs},
                          hmap::cmap::viridis);
}
