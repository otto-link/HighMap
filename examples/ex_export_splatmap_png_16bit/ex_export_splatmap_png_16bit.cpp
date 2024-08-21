#include "highmap.hpp"

int main(void)
{

  const hmap::Vec2<int>   shape = {512, 512};
  const hmap::Vec2<float> res = {4.f, 4.f};
  int                     seed = 2;

  hmap::Array zr = hmap::noise(hmap::NoiseType::PERLIN, shape, res, seed++);
  hmap::Array zg = hmap::noise(hmap::NoiseType::PERLIN, shape, res, seed++);
  hmap::Array zb = hmap::noise(hmap::NoiseType::PERLIN, shape, res, seed++);
  hmap::Array za = hmap::noise(hmap::NoiseType::PERLIN, shape, res, seed++);

  hmap::export_banner_png("ex_export_splatmap_png_16bit0.png",
                          {zr, zg, zb, za},
                          hmap::Cmap::GRAY);

  hmap::export_splatmap_png("ex_export_splatmap_png_16bit1.png",
                            &zr,
                            &zg,
                            &zb,
                            &za,
                            CV_16U);
}
