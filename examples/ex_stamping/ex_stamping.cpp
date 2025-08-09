#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 1;

  // kernel to be stamped
  hmap::Vec2<int> shape_kernel = {64, 64};
  hmap::Array     kernel = hmap::gabor(shape_kernel, 8.f, 15.f);

  // generate stamping locations
  size_t      n = 50;
  hmap::Cloud cloud = hmap::random_cloud(n, seed);

  // eventually stamp...
  hmap::Array z = hmap::stamping(shape,
                                 cloud.get_x(),
                                 cloud.get_y(),
                                 cloud.get_values(),
                                 kernel,
                                 32,   // kernel radius in pixels
                                 true, // scale ampl
                                 true, // scale radius
                                 hmap::StampingBlendMethod::MAXIMUM,
                                 ++seed,
                                 true,  // flip
                                 true); // rotate

  // export points to a cloud to generate a png file
  hmap::Array c = hmap::Array(shape);
  cloud.to_array(c);

  z.infos();

  c.to_png("ex_stamping0.png", hmap::Cmap::INFERNO);
  kernel.to_png("ex_stamping1.png", hmap::Cmap::INFERNO);
  z.to_png("ex_stamping2.png", hmap::Cmap::INFERNO);
}
