#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  float sigma = 1.f;

  auto z1 = z;
  auto z2 = z;
  auto z3 = z;
  auto z4 = z;
  hmap::falloff(z1, sigma, hmap::DistanceFunction::EUCLIDIAN);
  hmap::falloff(z2, sigma, hmap::DistanceFunction::EUCLISHEV);
  hmap::falloff(z3, sigma, hmap::DistanceFunction::CHEBYSHEV);
  hmap::falloff(z4, sigma, hmap::DistanceFunction::MANHATTAN);

  hmap::export_banner_png("ex_falloff.png",
                          {z, z1, z2, z3, z4},
                          hmap::Cmap::JET);
}
