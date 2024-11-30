#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  hmap::Vec2<int> tiling = {4, 4};
  float           overlap = 0.25f;

  hmap::Vec2<float> kw = {16.f, 16.f};
  int               seed = 1;

  // --- generate normal maps as RGBA heightmaps

  auto z1 = cone(shape);
  auto z2 = 0.1f * hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed++);

  hmap::Tensor n1 = hmap::normal_map(z1);
  hmap::Tensor n2 = hmap::normal_map(z2);

  auto h1 = hmap::HeightmapRGBA(shape,
                                tiling,
                                overlap,
                                n1.get_slice(0),
                                n1.get_slice(1),
                                n1.get_slice(2),
                                hmap::Array(shape, 1.f));

  auto h2 = hmap::HeightmapRGBA(shape,
                                tiling,
                                overlap,
                                n2.get_slice(0),
                                n2.get_slice(1),
                                n2.get_slice(2),
                                hmap::Array(shape, 1.f));

  float scaling = 1.f;

  auto ha = mix_normal_map_rgba(h1,
                                h2,
                                scaling,
                                hmap::NormalMapBlendingMethod::NMAP_LINEAR);
  auto hb = mix_normal_map_rgba(h1,
                                h2,
                                scaling,
                                hmap::NormalMapBlendingMethod::NMAP_DERIVATIVE);
  auto hc = mix_normal_map_rgba(h1,
                                h2,
                                scaling,
                                hmap::NormalMapBlendingMethod::NMAP_UDN);
  auto hd = mix_normal_map_rgba(h1,
                                h2,
                                scaling,
                                hmap::NormalMapBlendingMethod::NMAP_UNITY);
  auto he = mix_normal_map_rgba(h1,
                                h2,
                                scaling,
                                hmap::NormalMapBlendingMethod::NMAP_WHITEOUT);

  h1.to_png("ex_mix_normal_map_rgba0.png");
  h2.to_png("ex_mix_normal_map_rgba1.png");

  ha.to_png("ex_mix_normal_map_rgba2.png");
  hb.to_png("ex_mix_normal_map_rgba3.png");
  hc.to_png("ex_mix_normal_map_rgba4.png");
  hd.to_png("ex_mix_normal_map_rgba5.png");
  he.to_png("ex_mix_normal_map_rgba6.png");
}
