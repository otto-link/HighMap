#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 6;
  int             npoints = 10;

  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path        path = hmap::Path(npoints, seed, {1.3f, 1.7f, -0.2, 0.2f});
  path.reorder_nns();

  auto z1 = hmap::Array(shape);
  path.to_array(z1, bbox);

  auto z2 = hmap::Array(shape);
  {
    hmap::Path path_c = path;

    path_c.resample(0.05f);
    path_c.smooth();
    path_c.to_array(z2, bbox);
  }

  hmap::export_banner_png("ex_path_smooth.png", {z1, z2}, hmap::Cmap::INFERNO);
}
