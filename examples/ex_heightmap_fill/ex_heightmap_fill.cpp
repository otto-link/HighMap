#include "highmap.hpp"
#include "highmap/array.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<int>   tiling = {4, 2};
  float             overlap = 0.25;
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::HeightMap h = hmap::HeightMap(shape, tiling, overlap);

  hmap::fill(
      h,
      [&kw, &seed](hmap::Vec2<int>   shape,
                   hmap::Vec2<float> shift,
                   hmap::Vec2<float> scale) {
        return hmap::perlin(shape, kw, seed, nullptr, nullptr, shift, scale);
      });

  h.to_array().to_png("ex_heightmap_fill0.png", hmap::cmap::inferno);

  hmap::fill(h,
             [&kw, &seed](hmap::Vec2<int>   shape,
                          hmap::Vec2<float> shift,
                          hmap::Vec2<float> scale) {
               return hmap::perlin_mix(shape,
                                       kw,
                                       seed,
                                       nullptr,
                                       nullptr,
                                       shift,
                                       scale);
             });

  h.to_array().to_png("ex_heightmap_fill1.png", hmap::cmap::inferno);
}
