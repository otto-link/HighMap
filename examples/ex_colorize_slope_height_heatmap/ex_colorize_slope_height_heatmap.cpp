#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise(hmap::NoiseType::PERLIN, shape, kw, seed);

  hmap::Tensor col3 = hmap::colorize_slope_height_heatmap(z, hmap::Cmap::JET);
  col3.to_png("ex_colorize_slope_height_heatmap.png");
}
