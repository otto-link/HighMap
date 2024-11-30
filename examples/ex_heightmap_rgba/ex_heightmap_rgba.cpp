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
  auto zsp = hmap::slope(shape, 0.f, 1.f);
  auto zsm = hmap::slope(shape, 0.f, -1.f);

  hmap::remap(zr);
  hmap::remap(zg);
  hmap::remap(zb);
  hmap::remap(zsp);
  hmap::remap(zsm);

  hmap::export_banner_png("ex_heightmap_rgba0.png",
                          {zr, zg, zb, zsp},
                          hmap::Cmap::GRAY);

  auto hr = hmap::Heightmap(shape, tiling, overlap);
  auto hg = hmap::Heightmap(shape, tiling, overlap);
  auto hb = hmap::Heightmap(shape, tiling, overlap);
  auto ha = hmap::Heightmap(shape, tiling, overlap);
  auto hm = hmap::Heightmap(shape, tiling, overlap);

  hr.from_array_interp(zr);
  hg.from_array_interp(zg);
  hb.from_array_interp(zb);
  ha.from_array_interp(zsp);
  hm.from_array_interp(zsm);

  hmap::HeightmapRGBA h_rgba = hmap::HeightmapRGBA(hr, hg, hb, ha);
  h_rgba.to_png("ex_heightmap_rgba1.png");

  // colorize
  h_rgba.colorize(hr, hr.min(), 0.8f * hr.max(), hmap::Cmap::JET, &ha);
  h_rgba.to_png("ex_heightmap_rgba2.png");

  // mix
  auto h0 = hmap::Heightmap(shape, tiling, overlap, 0.f);
  auto h1 = hmap::Heightmap(shape, tiling, overlap, 1.f);

  hmap::HeightmapRGBA h_rgba1 = hmap::HeightmapRGBA(hr, hr, h0, h1);
  hmap::HeightmapRGBA h_rgba2 = hmap::HeightmapRGBA(h0, h0, hb, hm);

  auto hmix = mix_heightmap_rgba(h_rgba1, h_rgba2);
  hmix.to_png("ex_heightmap_rgba3.png");

  // mix list of heightmaps
  hmap::HeightmapRGBA h_rgba3 = hmap::HeightmapRGBA(h0, hr, h0, ha);

  std::vector<hmap::HeightmapRGBA *> list = {&h_rgba1, &h_rgba2, &h_rgba3};
  auto                               hmix_list = mix_heightmap_rgba(list, true);
  hmix_list.to_png("ex_heightmap_rgba4.png");

  h_rgba1.to_png("rgba1.png");
  h_rgba2.to_png("rgba2.png");
  h_rgba3.to_png("rgba3.png");
}
