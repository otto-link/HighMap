#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  uint              seed = 5;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::Array dz = hmap::gradient_talus(z);

  // normalize data to balance their respective weights during the
  // clustering
  hmap::remap(z);
  hmap::remap(dz);

  // --- build up 4 clusters based the elevation and the local talus
  // (to get fot instance for types of land: high-altitude flat,
  // high-altitude steep, low-altitude flat, low-altitude steep)
  int         nclusters = 4;
  hmap::Array labels4 = hmap::kmeans_clustering2(z, dz, nclusters);

  // (optional) continuous scoring can also be retrieved for each cluster
  std::vector<hmap::Array> scoring4;
  hmap::Array              aggregate_scoring4;
  labels4 = hmap::kmeans_clustering2(z,
                                     dz,
                                     nclusters,
                                     &scoring4,
                                     &aggregate_scoring4);

  // --- add some data mimicking some moisture increasing from west to east
  hmap::Array moisture = hmap::slope(shape, 0.f, 1.f);
  hmap::remap(moisture);

  // build up 6 clusters (cloud be biomes) based on dry/wet combined
  // with elevation/slope criteria
  nclusters = 6;
  std::vector<hmap::Array> scoring6;
  hmap::Array              labels6 = hmap::kmeans_clustering3(z,
                                                 dz,
                                                 moisture,
                                                 nclusters,
                                                 &scoring6);

  z.to_png("ex_kmeans_clustering0.png", hmap::Cmap::INFERNO);

  labels4.to_png("ex_kmeans_clustering1.png", hmap::Cmap::JET);

  hmap::export_banner_png("ex_kmeans_clustering2.png",
                          scoring4,
                          hmap::Cmap::NIPY_SPECTRAL,
                          false);

  aggregate_scoring4.to_png("ex_kmeans_clustering3.png", hmap::Cmap::JET);
  aggregate_scoring4.infos();

  labels6.to_png("ex_kmeans_clustering4.png", hmap::Cmap::JET);

  hmap::export_banner_png("ex_kmeans_clustering5.png",
                          scoring6,
                          hmap::Cmap::NIPY_SPECTRAL,
                          false);
}
