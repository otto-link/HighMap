#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  hmap::Vec2<int> tiling = {4, 4};
  float           overlap = 0.25f;

  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  auto zr = hmap::noise(hmap::NoiseType::PERLIN, shape, kw, seed++);
  auto zg = hmap::noise(hmap::NoiseType::PERLIN, shape, kw, seed++);
  auto zb = hmap::noise(hmap::NoiseType::PERLIN, shape, kw, seed++);

  hmap::remap(zr);
  hmap::remap(zg);
  hmap::remap(zb);

  hmap::export_banner_png("ex_heightmap_rgb0.png",
                          {zr, zg, zb},
                          hmap::Cmap::GRAY);

  auto zs = hmap::slope(shape, 0.f, 1.f);
  hmap::remap(zs);

  auto hr = hmap::Heightmap(shape, tiling, overlap);
  auto hg = hmap::Heightmap(shape, tiling, overlap);
  auto hb = hmap::Heightmap(shape, tiling, overlap);
  auto hs = hmap::Heightmap(shape, tiling, overlap);

  hr.from_array_interp(zr);
  hg.from_array_interp(zg);
  hb.from_array_interp(zb);
  hs.from_array_interp(zs);

  hmap::HeightmapRGB hrgb = hmap::HeightmapRGB(hr, hg, hb);
  hrgb.to_png("ex_heightmap_rgb1.png");

  // colorize
  hrgb.colorize(hr, hr.min(), 0.8f * hr.max(), hmap::Cmap::JET);
  hrgb.to_png("ex_heightmap_rgb2.png");

  // mix
  auto               h0 = hmap::Heightmap(shape, tiling, overlap);
  hmap::HeightmapRGB hrgb1 = hmap::HeightmapRGB(hr, hr, h0);
  hmap::HeightmapRGB hrgb2 = hmap::HeightmapRGB(h0, h0, hr);

  auto hmix = mix_heightmap_rgb(hrgb1, hrgb2, hs);
  hmix.to_png("ex_heightmap_rgb3.png");
}
