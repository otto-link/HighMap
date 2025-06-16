#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {8.f, 8.f};
  int               seed = 1;

  hmap::Vec2<float> jitter = {1.f, 1.f};
  float             k_smoothing = 0.5f;

  std::vector<hmap::VoronoiReturnType> types = {
      hmap::VoronoiReturnType::F1_SQUARED,
      hmap::VoronoiReturnType::F2_SQUARED,
      hmap::VoronoiReturnType::F1TF2_SQUARED,
      hmap::VoronoiReturnType::F1DF2_SQUARED,
      hmap::VoronoiReturnType::F2MF1_SQUARED,
      hmap::VoronoiReturnType::EDGE_DISTANCE_SQUARED};

  std::vector<hmap::Array> zs = {};

  for (auto type : types)
  {
    hmap::Array z = hmap::gpu::voronoi(shape,
                                       kw,
                                       seed,
                                       jitter,
                                       k_smoothing,
                                       type);
    hmap::remap(z);
    zs.push_back(z);
  }

  for (auto type : types)
  {
    hmap::Array z = hmap::gpu::voronoi_fbm(shape,
                                           kw,
                                           seed,
                                           jitter,
                                           k_smoothing,
                                           type);
    hmap::remap(z);
    zs.push_back(z);
  }

  hmap::export_banner_png("ex_voronoi.png", zs, hmap::Cmap::INFERNO);
}
