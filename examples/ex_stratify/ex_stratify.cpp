#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  hmap::Array mask = hmap::noise(hmap::NoiseType::PERLIN, shape, res, 0);
  hmap::remap(mask);

  // stratification features (nb of layers, elevations and gamma
  // correction factors)
  auto z1 = z;
  int  ns = 10;
  auto hs = hmap::linspace_jitted(z1.min(), z1.max(), ns, 0.9f, seed);
  auto gs = hmap::random_vector(0.2, 0.8, ns - 1, seed);

  hmap::stratify(z1, hs, gs);

  // oblique layers
  auto  z2 = z;
  float talus = 4.f / shape.x;
  float angle = 15.f;

  hmap::stratify_oblique(z2, &mask, hs, gs, talus, angle);

  z2.to_file("out.bin");

  hmap::export_banner_png("ex_stratify.png",
                          {z, z1, z2},
                          hmap::Cmap::TERRAIN,
                          true);
}
