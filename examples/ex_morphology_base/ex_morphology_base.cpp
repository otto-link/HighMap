#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::clamp_min(z, 0.f);
  hmap::remap(z);

  // radius = 1 ==> 3x3 square kernel
  int ir = 3;

  hmap::Array zd = hmap::dilation(z, ir);
  hmap::Array ze = hmap::erosion(z, ir);
  hmap::Array zc = hmap::closing(z, ir);
  hmap::Array zo = hmap::opening(z, ir);
  hmap::Array zg = hmap::morphological_gradient(z, ir);
  hmap::Array zt = hmap::morphological_top_hat(z, ir);
  hmap::Array zb = hmap::morphological_black_hat(z, ir);

  hmap::make_binary(z);
  hmap::Array zr = hmap::border(z, ir);

  hmap::export_banner_png("ex_morphology_base.png",
                          {z, zd, ze, zc, zo, zg, zt, zb, zr},
                          hmap::Cmap::GRAY);
}
