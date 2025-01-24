#include "highmap.hpp"

void helper_ij_to_array(hmap::Array                         &w,
                        const std::vector<std::vector<int>> &i_list,
                        const std::vector<std::vector<int>> &j_list,
                        float                                value = 1.f)
{
  for (size_t r = 0; r < i_list.size(); r++)
    for (size_t k = 0; k < i_list[r].size(); k++)
      w(i_list[r][k], j_list[r][k]) = value;
}

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  hmap::Vec2<int>              ij_start = {40, 40};
  std::vector<hmap::Vec2<int>> ij_end_list = {{230, 230}, {0, 128}};

  // --- Dijkstra

  std::vector<std::vector<int>> id, jd;

  hmap::find_path_dijkstra(z, ij_start, ij_end_list, id, jd);

  hmap::Array zd = z;
  helper_ij_to_array(zd, id, jd);

  hmap::export_banner_png("ex_find_path.png", {z, zd}, hmap::Cmap::INFERNO);
}
