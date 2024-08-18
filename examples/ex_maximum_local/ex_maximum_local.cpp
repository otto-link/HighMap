#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;
  int               radius = 5;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::Array zmin = hmap::minimum_local(z, radius);
  hmap::Array zmax = hmap::maximum_local(z, radius);
  hmap::Array zdisk = hmap::maximum_local_disk(z, radius);

  hmap::export_banner_png("ex_maximum_local.png",
                          {z, zmin, zmax, zdisk},
                          hmap::Cmap::VIRIDIS);
}
