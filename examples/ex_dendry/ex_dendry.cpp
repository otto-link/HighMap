#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  // control function based on an array
  auto control_hmap = hmap::noise(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::remap(control_hmap, 0.5f, 1.f);

  hmap::Vec2<float> kd = {8.f, 8.f};
  auto              z1 = hmap::dendry(shape, kd, seed, control_hmap);
  hmap::remap(z1);

  // array-based noise function can also be used
  // hmap::ArrayFunction p = hmap::ArrayFunction(z, {1.f, 1.f}, true);

  hmap::PerlinFunction p = hmap::PerlinFunction(kw, seed);
  auto                 z2 = hmap::dendry(shape, kd, seed, p, 1.f, 0.5f);
  hmap::remap(z2);

  hmap::export_banner_png("ex_dendry.png",
                          {control_hmap, z1, z2},
                          hmap::Cmap::VIRIDIS);
}
