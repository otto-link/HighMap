#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  float sigma = 0.2f;
  int   iterations = 10;

  auto z1 = z;
  hmap::laplace(z1, sigma, iterations);

  auto  z2 = z;
  float talus = 1.f / (float)shape[0];
  hmap::laplace_edge_preserving(z2, talus, sigma, iterations);

  hmap::export_banner_png("ex_laplace.png",
                          {z, z1, z2},
                          hmap::cmap::viridis,
                          true);
}
