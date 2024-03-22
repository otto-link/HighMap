#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::n_perlin, shape, res, seed);
  int         nbuffer = 64;

  auto zp = z;
  hmap::make_periodic(zp, nbuffer);

  // do some tiling to check periodicity
  auto zt = zp;
  zt = hstack(zt, zt);
  zt = vstack(zt, zt);

  hmap::export_banner_png("ex_make_periodic0.png",
                          {z, zp},
                          hmap::cmap::viridis);

  zt.to_png("ex_make_periodic1.png", hmap::cmap::viridis);
}
