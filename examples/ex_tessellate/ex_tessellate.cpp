#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z1 = hmap::noise(hmap::NoiseType::PERLIN, shape, res, seed);

  // uniform
  hmap::Array z2 = hmap::tessellate(z1, seed++, 0.005f);

  // density driven by inverse gradient norm
  hmap::Array d = hmap::gradient_norm(z1);

  hmap::remap(d, 0.01f, 1.f);
  d = 1.f / (d * d);
  hmap::remap(d);

  hmap::Array z3 = hmap::tessellate(z1, seed++, 0.005f, &d);

  hmap::export_banner_png("ex_tessellate.png",
                          {z1, z2, z3},
                          hmap::Cmap::JET,
                          true);
}
