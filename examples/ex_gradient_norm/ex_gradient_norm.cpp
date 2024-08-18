#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  auto dz = hmap::gradient_norm(z);
  auto dz_p = hmap::gradient_norm_prewitt(z);
  auto dz_c = hmap::gradient_norm_scharr(z);
  auto dz_s = hmap::gradient_norm_sobel(z);

  hmap::export_banner_png("ex_gradient_norm.png",
                          {dz, dz_p, dz_c, dz_s},
                          hmap::Cmap::VIRIDIS);
}
