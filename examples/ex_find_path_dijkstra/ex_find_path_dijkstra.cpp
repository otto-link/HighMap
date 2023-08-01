#include <vector>

#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 2;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  z.to_png("ex_find_path_dijkstra0.png", hmap::cmap::terrain, true);
  hmap::remap(z);

  hmap::Vec2<int> ij_start = {10, 20};
  hmap::Vec2<int> ij_end = {200, 240};

  std::vector<int> i, j;

  z.find_path_dijkstra(ij_start, ij_end, i, j);

  // export path to a png file
  hmap::Array w = hmap::Array(shape);
  for (size_t k = 0; k < i.size(); k++)
    w(i[k], j[k]) = z(i[k], j[k]);

  w.to_png("ex_find_path_dijkstra1.png", hmap::cmap::jet);
}
