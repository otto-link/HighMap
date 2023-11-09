#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  hmap::Vec2<int> tiling = {4, 4};
  float           overlap = 0.25f;

  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  auto zr = hmap::perlin(shape, kw, seed++);
  auto zg = hmap::perlin(shape, kw, seed++);
  auto zb = hmap::perlin(shape, kw, seed++);

  hmap::remap(zr);
  hmap::remap(zg);
  hmap::remap(zb);

  hmap::export_banner_png("ex_heightmap_rgb0.png",
                          {zr, zg, zb},
                          hmap::cmap::gray);

  auto zs = hmap::slope_x(shape, 1.f);
  hmap::remap(zs);

  auto hr = hmap::HeightMap(shape, tiling, overlap);
  auto hg = hmap::HeightMap(shape, tiling, overlap);
  auto hb = hmap::HeightMap(shape, tiling, overlap);
  auto hs = hmap::HeightMap(shape, tiling, overlap);

  hr.from_array_interp(zr);
  hg.from_array_interp(zg);
  hb.from_array_interp(zb);
  hs.from_array_interp(zs);

  hmap::HeightMapRGB hrgb = hmap::HeightMapRGB(hr, hg, hb);
  hrgb.to_png_16bit("ex_heightmap_rgb1.png");

  // colorize
  hrgb.colorize(hr, hr.min(), 0.8f * hr.max(), hmap::cmap::jet);
  hrgb.to_png_16bit("ex_heightmap_rgb2.png");

  // mix
  auto               h0 = hmap::HeightMap(shape, tiling, overlap);
  hmap::HeightMapRGB hrgb1 = hmap::HeightMapRGB(hr, hr, h0);
  hmap::HeightMapRGB hrgb2 = hmap::HeightMapRGB(h0, h0, hr);

  auto hmix = mix_heightmap_rgb(hrgb1, hrgb2, hs);
  hmix.to_png_16bit("ex_heightmap_rgb3.png");
}
