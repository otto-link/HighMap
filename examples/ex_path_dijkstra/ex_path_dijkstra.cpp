#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;
  hmap::Array       z = hmap::fbm_perlin(shape, res, seed);
  hmap::remap(z);

  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path        path = hmap::Path(3, seed, {1.2f, 1.8f, -0.3, 0.3f});
  path.reorder_nns();
  path.closed = false;

  // before
  auto z1 = hmap::Array(shape);
  path.to_array(z1, bbox);

  // after
  int edge_divisions = 20;
  path.dijkstra(z, bbox, edge_divisions);

  auto z2 = hmap::Array(shape);
  path.to_array(z2, bbox);

  hmap::export_banner_png("ex_path_dijkstra.png",
                          {z, z1, z2},
                          hmap::cmap::inferno);
}
