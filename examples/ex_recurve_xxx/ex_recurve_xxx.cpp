#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::remap(z);

  auto zs = z;
  auto zb = z;
  auto ze = z;
  auto zr = z;

  hmap::recurve_s(zs);                         // "gentle S"
  hmap::recurve_bexp(zb, 0.5f);                // bumpy exponential
  hmap::recurve_exp(ze, 0.5f);                 // sharp exponential
  hmap::recurve_smoothstep_rational(zr, 0.5f); // general smoothstep

  hmap::export_banner_png("ex_recurve_xxx.png",
                          {z, zs, zb, ze, zr},
                          hmap::cmap::terrain);
}
