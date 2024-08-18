#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {128, 128};
  int             seed = 1;

  hmap::Array z1 = hmap::noise(hmap::NoiseType::PERLIN,
                               shape,
                               {8.f, 8.f},
                               seed);
  hmap::Array z2 = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                   shape,
                                   {4.f, 4.f},
                                   seed + 1);
  hmap::Array z3 = hmap::white(shape, 0.f, 1.f, seed + 2);

  hmap::Array t = hmap::noise(hmap::NoiseType::PERLIN,
                              shape,
                              {2.f, 2.f},
                              seed + 3); // mixer

  hmap::remap(t);

  auto zm = hmap::mixer(t, {&z1, &z2, &z3});

  hmap::export_banner_png("ex_mixer.png",
                          {t, z1, z2, z3, zm},
                          hmap::Cmap::VIRIDIS);
}
