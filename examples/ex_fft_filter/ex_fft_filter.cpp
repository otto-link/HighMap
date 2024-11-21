#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {32.f, 32.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::SIMPLEX2, shape, res, seed);

  float       kc = 32.f;
  hmap::Array zf = fft_filter(z, kc);

  bool        smooth_cutoff = true;
  hmap::Array zs = fft_filter(z, kc, smooth_cutoff);

  hmap::Array mz = hmap::fft_modulus(z);
  hmap::remap(mz, 0.001f, 1.f);
  mz = hmap::log10(mz);

  hmap::Array mzf = hmap::fft_modulus(zf);
  hmap::remap(mzf, 0.001f, 1.f);
  mzf = hmap::log10(mzf);

  hmap::Array mzs = hmap::fft_modulus(zs);
  hmap::remap(mzs, 0.001f, 1.f);
  mzs = hmap::log10(mzs);

  hmap::remap(z);
  hmap::remap(zf);
  hmap::remap(zs);
  hmap::remap(mz);
  hmap::remap(mzf);
  hmap::remap(mzs);

  hmap::export_banner_png("ex_fft_filter.png",
                          {z, zf, zs, mz, mzf, mzs},
                          hmap::Cmap::INFERNO);
}
