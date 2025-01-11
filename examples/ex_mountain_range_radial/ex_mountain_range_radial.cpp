#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int> shape = {256, 256};
  shape = {1024, 1024};
  hmap::Vec2<float> kw = {8.f, 8.f};
  int               seed = 1;

  hmap::Array z1 = hmap::gpu::mountain_range_radial(shape, kw, seed);

  hmap::export_banner_png("ex_mountain_range_radial.png",
                          {z1},
                          hmap::Cmap::JET,
                          true);
}
