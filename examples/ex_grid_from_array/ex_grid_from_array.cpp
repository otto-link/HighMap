#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 1;

  hmap::Array z = hmap::white_sparse_binary(shape, 1e-4f, seed);

  std::vector<float> x;
  std::vector<float> y;
  hmap::Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f};

  hmap::grid_from_array(z, x, y, bbox);

  hmap::Cloud cloud = hmap::Cloud(x, y, 1.f);
  auto        zc = hmap::Array(shape);
  cloud.to_array(zc, bbox);

  hmap::export_banner_png("ex_grid_from_array.png", {z, zc}, hmap::cmap::gray);
}
