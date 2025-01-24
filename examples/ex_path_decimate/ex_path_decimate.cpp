#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 3;

  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path        path = hmap::Path(20, seed, {1.2f, 1.8f, -0.3, 0.3f});
  path.reorder_nns();
  path.fractalize(1, seed);
  // path.closed = true;

  int ntarget = 15;

  auto z1 = hmap::Array(shape);
  path.to_array(z1, bbox);

  // Visvalingam-Whyatt algo
  auto       z2 = hmap::Array(shape);
  hmap::Path path2 = path;
  path2.decimate_vw(ntarget);
  path2.to_array(z2, bbox);

  // similar but curvature-based
  auto z3 = hmap::Array(shape);
  path2 = path;
  path2.decimate_cfit(ntarget);
  path2.to_array(z3, bbox);

  hmap::export_banner_png("ex_path_decimate.png",
                          {z1, z2, z3},
                          hmap::Cmap::INFERNO);
}
