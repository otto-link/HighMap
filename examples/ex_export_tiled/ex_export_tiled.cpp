#include "highmap.hpp"

int main(void)
{

  const hmap::Vec2<int>   shape = {763, 451};
  const hmap::Vec2<float> res = {1.f, 4.f};
  int                     seed = 0;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  hmap::Vec2<int> tiling = {4, 3};
  std::string     fname_radical = "ex_export_tiled";
  std::string     fname_extension = "png";
  int             leading_zeros = 2;
  int             depth = CV_16U;
  bool            overlapping_edges = false;
  bool            reverse_tile_y_indexing = false;

  hmap::export_tiled(fname_radical,
                     fname_extension,
                     z,
                     tiling,
                     leading_zeros,
                     depth,
                     overlapping_edges,
                     reverse_tile_y_indexing);
}
