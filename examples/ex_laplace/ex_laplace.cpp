#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  float sigma = 0.2f;
  int   iterations = 10;

  auto z1 = z;
  hmap::laplace(z1, sigma, iterations);

  auto  z2 = z;
  float talus = 1.f / (float)shape.x;
  hmap::laplace_edge_preserving(z2, talus, sigma, iterations);

  hmap::export_banner_png("ex_laplace.png",
                          {z, z1, z2},
                          hmap::Cmap::VIRIDIS,
                          true);
}
