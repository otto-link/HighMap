#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {128, 128};
  int             seed = 1;

  hmap::Array z1 = hmap::noise(hmap::NoiseType::PERLIN,
                               shape,
                               {2.f, 2.f},
                               seed);
  hmap::Array z2 = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                   shape,
                                   {4.f, 4.f},
                                   seed + 1);

  hmap::remap(z1);
  hmap::remap(z2);

  auto z_exclusion = hmap::blend_exclusion(z1, z2);
  auto z_negate = hmap::blend_negate(z1, z2);
  auto z_overlay = hmap::blend_overlay(z1, z2);
  auto z_soft = hmap::blend_soft(z1, z2);
  auto z_gradients = hmap::blend_gradients(z1, z2);

  hmap::export_banner_png("ex_blend0.png",
                          {z1, z2, z_exclusion},
                          hmap::Cmap::VIRIDIS);

  hmap::export_banner_png("ex_blend1.png",
                          {z1, z2, z_negate},
                          hmap::Cmap::VIRIDIS);

  hmap::export_banner_png("ex_blend2.png",
                          {z1, z2, z_overlay},
                          hmap::Cmap::VIRIDIS);

  hmap::export_banner_png("ex_blend3.png",
                          {z1, z2, z_soft},
                          hmap::Cmap::VIRIDIS);

  hmap::export_banner_png("ex_blend4.png",
                          {z1, z2, z_gradients},
                          hmap::Cmap::VIRIDIS);
}
