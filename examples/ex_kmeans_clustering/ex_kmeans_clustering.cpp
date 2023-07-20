#include "highmap.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  uint                     seed = 5;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::Array dz = hmap::gradient_talus(z);

  // normalize data to balance their respective weights during the
  // clustering
  hmap::remap(z);
  hmap::remap(dz);

  // build up 4 clusters based the elevation and the local talus (to
  // get fot instance for types of land: high-altitude flat,
  // high-altitude steep, low-altitude flat, low-altitude steep)
  int         nclusters = 4;
  hmap::Array labels = hmap::kmeans_clustering2(z, dz, nclusters);

  z.to_png("ex_kmeans_clustering0.png", hmap::cmap::inferno);
  labels.to_png("ex_kmeans_clustering1.png", hmap::cmap::nipy_spectral);
}
