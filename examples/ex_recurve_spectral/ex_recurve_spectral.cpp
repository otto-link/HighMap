#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 0;

  hmap::Array z = hmap::white(shape, 0.f, 1.d, seed);
  hmap::remap(z);

  // weights applied to the spectral content (weight actual values are
  // interpolated)
  std::vector<float> weights = {};

  auto z1 = z;
  weights = {1.f, 0.f, 1.f}; // notch filter
  hmap::recurve_spectral(z1, weights);

  auto z2 = z;
  weights = {0.f, 0.f, 0.f, 1.f}; // high-pass
  hmap::recurve_spectral(z2, weights);

  auto z3 = z;
  weights = {1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}; // low-pass
  hmap::recurve_spectral(z3, weights);

  hmap::export_banner_png("ex_recurve_spectral.png",
                          {z, z1, z2, z3},
                          hmap::Cmap::NIPY_SPECTRAL);
}
