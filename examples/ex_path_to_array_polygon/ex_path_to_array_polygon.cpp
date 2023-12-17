#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 1;

  hmap::Vec4<float> bbox_rand = {0.2f, 0.8f, 0.2f, 0.8f};
  hmap::Path        path = hmap::Path(5, seed, bbox_rand);
  path.reorder_nns();

  hmap::Vec4<float> bbox = {-0.5f, 1.5f, -0.5f, 1.5f};

  auto z = path.to_array_polygon(shape, bbox);

  z.to_png("ex_path_to_array_polygon.png", hmap::cmap::inferno);
}
