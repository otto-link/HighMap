#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<int>   tiling = {4, 2};
  float             overlap = 0.25;
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array     z = hmap::noise(hmap::NoiseType::PERLIN,
                              hmap::Vec2<int>(64, 64),
                              kw,
                              seed);
  hmap::Heightmap h = hmap::Heightmap(shape, tiling, overlap);

  h.from_array_interp_bicubic(z);

  z.to_png("ex_heightmap_from_array0.png", hmap::Cmap::INFERNO);
  h.to_array().to_png("ex_heightmap_from_array1.png", hmap::Cmap::INFERNO);
}
