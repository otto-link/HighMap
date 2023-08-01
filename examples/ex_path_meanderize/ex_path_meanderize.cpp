#include "highmap/array.hpp"
#include "highmap/geometry.hpp"
#include "highmap/io.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 1;

  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path        path = hmap::Path(4, seed, {1.3f, 1.7f, -0.2, 0.2f});
  path.reorder_nns();
  path.closed = true;

  auto z1 = hmap::Array(shape);
  path.to_array(z1, bbox);

  auto z2 = hmap::Array(shape);
  path.bezier(0.5f, 40);
  path.to_array(z2, bbox);

  auto z3 = hmap::Array(shape);
  {
    float radius = 0.2f;
    float tangent_ratio = 0.2f;
    path.meanderize(radius, tangent_ratio);
  }
  path.to_array(z3, bbox);

  hmap::export_banner_png("ex_path_meanderize.png",
                          {z1, z2, z3},
                          hmap::cmap::inferno);
}
