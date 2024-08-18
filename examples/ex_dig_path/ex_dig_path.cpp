#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;
  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path        path = hmap::Path(5, seed, {1.1f, 1.9f, -0.4, 0.4f});
  path.closed = false;
  path.reorder_nns();

  path.dijkstra(z, bbox, 0, 0.9f, 1.f);

  auto z1 = hmap::Array(shape);
  path.to_array(z1, bbox);

  auto z2 = z;
  int  width = 1; // pixels
  int  decay = 2;
  int  flattening_radius = 16;
  bool force_downhill = false;

  hmap::dig_path(z2,
                 path,
                 width,
                 decay,
                 flattening_radius,
                 force_downhill,
                 bbox);

  auto z3 = z;
  force_downhill = true;

  hmap::dig_path(z3,
                 path,
                 width,
                 decay,
                 flattening_radius,
                 force_downhill,
                 bbox);

  hmap::export_banner_png("ex_dig_path.png",
                          {z, z1, z2, z3},
                          hmap::Cmap::TERRAIN,
                          true);
}
