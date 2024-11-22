#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<int>   tiling = {4, 2};
  float             overlap = 0.25;
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Heightmap h = hmap::Heightmap(shape, tiling, overlap);

  hmap::fill(h,
             [&kw, &seed](hmap::Vec2<int> shape, hmap::Vec4<float> bbox)
             {
               return hmap::noise(hmap::NoiseType::PERLIN,
                                  shape,
                                  kw,
                                  seed,
                                  nullptr,
                                  nullptr,
                                  nullptr,
                                  bbox);
             });

  h.to_array().to_png("ex_heightmap_fill0.png", hmap::Cmap::INFERNO);
}
