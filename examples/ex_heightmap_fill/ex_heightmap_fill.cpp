#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<int>   tiling = {4, 2};
  float             overlap = 0.25;
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;
  int               count = 0;
  std::string       fname;

  hmap::HeightMap h = hmap::HeightMap(shape, tiling, overlap);

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

  fname = "ex_heightmap_fill" + std::to_string(count++) + ".png";
  h.to_array().to_png(fname.c_str(), hmap::cmap::inferno);
}
