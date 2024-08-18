#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  auto zs = z;
  auto zb = z;
  auto ze = z;
  auto zk = z;
  auto zr = z;

  hmap::recurve_s(zs);                         // "gentle S"
  hmap::recurve_bexp(zb, 0.5f);                // bumpy exponential
  hmap::recurve_exp(ze, 0.5f);                 // sharp exponential
  hmap::recurve_kura(zk, 2.f, 0.5f);           // Kumaraswamy
  hmap::recurve_smoothstep_rational(zr, 0.5f); // general smoothstep

  hmap::export_banner_png("ex_recurve_xxx.png",
                          {z, zs, zb, ze, zk, zr},
                          hmap::Cmap::TERRAIN);
}
