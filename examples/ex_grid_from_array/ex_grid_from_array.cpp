#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 1;

  hmap::Array z = hmap::white_sparse(shape, 0.1f, 1.f, 5e-4f, seed);

  std::vector<float> x, y, value;
  hmap::Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f};

  hmap::grid_from_array(z, x, y, value, bbox);

  hmap::Cloud cloud = hmap::Cloud(x, y, value);
  auto        zc = hmap::Array(shape);
  cloud.to_array(zc, bbox);

  hmap::export_banner_png("ex_grid_from_array.png",
                          {z, z},
                          hmap::Cmap::NIPY_SPECTRAL);
}
