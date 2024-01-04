#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  uint            seed = 1;

  auto noise = hmap::fbm_perlin(shape, {2, 2}, seed);
  hmap::remap(noise, 0.f, 0.1f);

  // circle
  float radius = 0.25f;
  auto  z1 = hmap::sdf_circle(shape, radius);

  // polygon
  hmap::Vec4<float> bbox = {0.2f, 0.8f, 0.2f, 0.8f};
  hmap::Path        path = hmap::Path(5, seed, bbox);
  path.reorder_nns();

  auto z2 = hmap::sdf_polygon(shape, path.get_x(), path.get_y(), &noise);

  float width = 0.1f;
  auto z3 = hmap::sdf_polygon_annular(shape, path.get_x(), path.get_y(), width);

  // path ("open polygon")
  auto z4 = hmap::sdf_path(shape, path.get_x(), path.get_y());

  hmap::export_banner_png("ex_sdf.png", {z1, z2, z3, z4}, hmap::cmap::jet);
}
