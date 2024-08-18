#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array dr = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::Array dx, dy;

  hmap::radial_displacement_to_xy(dr, dx, dy);

  hmap::export_banner_png("ex_radial_displacement_to_xy.png",
                          {dr, dx, dy},
                          hmap::Cmap::JET);
}
