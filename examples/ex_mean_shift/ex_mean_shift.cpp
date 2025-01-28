#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);

  int   ir = 32;
  float talus = 16.f / (float)shape.x;
  int   iterations = 4;

  hmap::Array zm = hmap::mean_shift(z, ir, talus, iterations);

  zm.to_png_grayscale("out.png", CV_16U);

  hmap::export_banner_png("ex_mean_shift.png", {z, zm}, hmap::Cmap::JET);
}
