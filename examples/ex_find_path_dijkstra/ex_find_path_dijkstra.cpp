#include <vector>

#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  z.to_png("ex_find_path_dijkstra0.png", hmap::Cmap::TERRAIN, true);

  hmap::Vec2<int> ij_start = {40, 40};
  hmap::Vec2<int> ij_end = {230, 230};

  std::vector<int> i, j;

  hmap::find_path_dijkstra(z, ij_start, ij_end, i, j);

  // export path to a png file
  hmap::Array w = hmap::Array(shape);
  for (size_t k = 0; k < i.size(); k++)
    w(i[k], j[k]) = 1.f;

  w.to_png("ex_find_path_dijkstra1.png", hmap::Cmap::GRAY);

  // set "elevation_ratio" to 1.f to find the path with the lowest
  // cumulative elevation
  hmap::find_path_dijkstra(z, ij_start, ij_end, i, j, 1.f);

  // export path to a png file
  w = hmap::Array(shape);
  for (size_t k = 0; k < i.size(); k++)
    w(i[k], j[k]) = 1.f;

  w.to_png("ex_find_path_dijkstra2.png", hmap::Cmap::GRAY);
}
