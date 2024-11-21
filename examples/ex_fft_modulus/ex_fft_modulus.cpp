#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {128.f, 128.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::SIMPLEX2, shape, res, seed);

  hmap::Array modulus = hmap::fft_modulus(z);
  hmap::remap(modulus, 0.001f, 1.f);
  modulus = hmap::log10(modulus);

  hmap::remap(z);
  hmap::remap(modulus);

  hmap::export_banner_png("ex_fft_modulus.png",
                          {z, modulus},
                          hmap::Cmap::INFERNO);
}
