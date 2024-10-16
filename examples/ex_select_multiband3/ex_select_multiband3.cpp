#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  hmap::Array band_low;
  hmap::Array band_mid;
  hmap::Array band_high;

  float ratio1 = 0.2f;
  float ratio2 = 0.5f;
  float overlap = 0.5f;

  hmap::select_multiband3(z,
                          band_low,
                          band_mid,
                          band_high,
                          ratio1,
                          ratio2,
                          overlap);

  hmap::export_banner_png("ex_select_multiband3.png",
                          {z, band_low, band_mid, band_high},
                          hmap::Cmap::MAGMA);
}
