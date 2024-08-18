#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 2;
  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path        path = hmap::Path(3, seed, {1.1f, 1.9f, -0.4, 0.4f});
  path.closed = false;
  path.reorder_nns();
  path.set_values_from_array(z, bbox);

  // before
  auto z1 = hmap::Array(shape);
  path.to_array(z1, bbox);

  // after
  int edge_divisions = 0;
  path.dijkstra(z, bbox, edge_divisions, 0.9f);
  path.set_values_from_array(z, bbox);

  auto z2 = hmap::Array(shape);
  path.to_array(z2, bbox);

  hmap::export_banner_png("ex_path_dijkstra.png",
                          {z, z1, z2},
                          hmap::Cmap::INFERNO);
}
