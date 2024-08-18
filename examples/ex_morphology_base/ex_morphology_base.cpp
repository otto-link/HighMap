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

  hmap::Array zd = dilation(z, ir);
  hmap::Array ze = erosion(z, ir);
  hmap::Array zc = closing(z, ir);
  hmap::Array zo = opening(z, ir);
  hmap::Array zg = morphological_gradient(z, ir);
  hmap::Array zt = morphological_top_hat(z, ir);
  hmap::Array zb = morphological_black_hat(z, ir);

  hmap::export_banner_png("ex_morphology_base.png",
                          {z, zd, ze, zc, zo, zg, zt, zb},
                          hmap::Cmap::GRAY);
}
