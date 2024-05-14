#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array noise = 0.2f * hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                             shape,
                                             res,
                                             seed);

  float slope = 1.f;

  hmap::Array oblique = hmap::slope(shape, 30.f, slope, &noise);

  hmap::Array valley = maximum_smooth(
      hmap::slope(shape, 0.f, slope, &noise, &noise),
      hmap::slope(shape, 90.f, slope, &noise, &noise),
      0.1f);

  hmap::export_banner_png("ex_slope.png",
                          {oblique, valley},
                          hmap::cmap::terrain);
}
