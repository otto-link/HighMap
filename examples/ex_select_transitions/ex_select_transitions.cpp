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
                                   ++seed);

  hmap::remap(z1);
  hmap::remap(z2);

  hmap::Array z_m = hmap::minimum(z1, z2);              // sharp transition
  hmap::Array z_s = hmap::minimum_smooth(z1, z2, 0.2f); // smooth transition

  hmap::Array mask_m = hmap::select_transitions(z1, z2, z_m);
  hmap::Array mask_s = hmap::select_transitions(z1, z2, z_s);

  hmap::export_banner_png("ex_select_transitions0.png",
                          {z1, z2},
                          hmap::Cmap::VIRIDIS);

  hmap::export_banner_png("ex_select_transitions1.png",
                          {z_m, z_s},
                          hmap::Cmap::VIRIDIS);

  hmap::export_banner_png("ex_select_transitions2.png",
                          {mask_m, mask_s},
                          hmap::Cmap::GRAY);
}
