#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  int         nbuffer = 64;

  auto zp = z;
  hmap::make_periodic(zp, nbuffer);

  // do some tiling to check periodicity
  auto zt = zp;
  zt = hstack(zt, zt);
  zt = vstack(zt, zt);

  hmap::export_banner_png("ex_make_periodic0.png",
                          {z, zp},
                          hmap::Cmap::VIRIDIS);

  zt.to_png("ex_make_periodic1.png", hmap::Cmap::VIRIDIS);
}
