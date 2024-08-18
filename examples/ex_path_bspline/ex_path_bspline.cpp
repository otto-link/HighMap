#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 6;

  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path        path = hmap::Path(10, seed, {1.2f, 1.8f, -0.3, 0.3f});
  path.reorder_nns();

  auto z1 = hmap::Array(shape);
  path.to_array(z1, bbox);

  auto z2 = hmap::Array(shape);
  path.bspline();
  path.to_array(z2, bbox);

  hmap::export_banner_png("ex_path_bspline.png", {z1, z2}, hmap::Cmap::INFERNO);
}
