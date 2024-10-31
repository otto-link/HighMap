#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  // unwrapped reference
  hmap::Array alpha0 = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                       shape,
                                       res,
                                       seed);
  hmap::remap(alpha0, 0.f, 4.f * M_PI);

  // wrap it
  hmap::Array alpha = alpha0;
  for (auto &v : alpha.vector)
    v = std::fmod(v, 2.f * M_PI);

  // unwrap (and shift zero reference)
  hmap::Array alpha_uw = hmap::unwrap_phase(alpha);
  alpha_uw = alpha_uw - alpha_uw.min() + alpha0.min();

  hmap::export_banner_png("ex_unwrap_phase.png",
                          {alpha0, alpha, alpha_uw},
                          hmap::Cmap::INFERNO);
}
