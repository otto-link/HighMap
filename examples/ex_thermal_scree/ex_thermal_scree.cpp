#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap ::remap(z);
  auto z0 = z;

  float talus = 2.f / shape.x;
  float zmax = 0.5f;
  float noise_ratio = 0.5f;

  hmap::thermal_scree(z, talus, seed, zmax, noise_ratio);

  auto            zf = z0;
  hmap::Vec2<int> shape_coarse = {64, 64};
  hmap::thermal_scree_fast(zf, shape_coarse, talus, seed, zmax, noise_ratio);

  hmap::export_banner_png("ex_thermal_scree.png",
                          {z0, z, zf},
                          hmap::Cmap::TERRAIN,
                          true);
}
