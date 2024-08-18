#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array noise = 0.2f *
                      hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);

  float slope = 1.f;

  hmap::Array oblique = hmap::slope(shape, 30.f, slope, nullptr, &noise);

  hmap::Array valley = maximum_smooth(
      hmap::slope(shape, 0.f, slope, nullptr, &noise, &noise),
      hmap::slope(shape, 90.f, slope, nullptr, &noise, &noise),
      0.1f);

  // with control array
  hmap::Array ctrl_array = hmap::noise(hmap::NoiseType::PERLIN,
                                       shape,
                                       kw,
                                       seed);
  hmap::remap(ctrl_array, 0.8f, 1.2f);

  hmap::Array wctrl = hmap::slope(shape, 30.f, slope, &ctrl_array);

  hmap::export_banner_png("ex_slope.png",
                          {oblique, valley, wctrl},
                          hmap::Cmap::TERRAIN);
}
