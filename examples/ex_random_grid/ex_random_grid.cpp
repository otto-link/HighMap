#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  int               seed = 1;
  hmap::Vec4<float> bbox = {-1.f, 2.f, 0.f, 3.f};

  std::vector<float> x, y, v;

  hmap::Vec2<float> delta = {0.1f, 0.2f};
  hmap::Vec2<float> stagger_ratio = {0.5f, 0.f};
  hmap::Vec2<float> jitter_ratio = {0.3f, 0.3f};

  hmap::random_grid(x, y, v, seed, delta, stagger_ratio, jitter_ratio, bbox);

  // generate png
  {
    hmap::Cloud cloud = hmap::Cloud(x, y, v);
    hmap::Array array = hmap::Array(shape);
    cloud.to_array(array, bbox);
    array.to_png("ex_random_grid.png", hmap::Cmap::VIRIDIS);
  }
}
