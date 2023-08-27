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

  hmap::fill(
      h,
      [&kw, &seed](hmap::Vec2<int>   shape,
                   hmap::Vec2<float> shift,
                   hmap::Vec2<float> scale) {
        return hmap::perlin(shape, kw, seed, nullptr, nullptr, shift, scale);
      });

  fname = "ex_heightmap_fill" + std::to_string(count++) + ".png";
  h.to_array().to_png(fname.c_str(), hmap::cmap::inferno);

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

  fname = "ex_heightmap_fill" + std::to_string(count++) + ".png";
  h.to_array().to_png(fname.c_str(), hmap::cmap::inferno);

  hmap::fill(h,
             [&kw, &seed](hmap::Vec2<int> shape)
             { return hmap::bump_field(shape, kw.x, seed, 1.f); });
  h.smooth_overlap_buffers();

  fname = "ex_heightmap_fill" + std::to_string(count++) + ".png";
  h.to_array().to_png(fname.c_str(), hmap::cmap::inferno);
}
