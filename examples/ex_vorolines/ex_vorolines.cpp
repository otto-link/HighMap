#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int> shape = {256, 256};
  int             seed = 1;
  float           density = 8.f;
  float           k_smoothing = 0.005f;
  float           exp_sigma = 0.01f;
  float           alpha = 0.f;
  float           alpha_span = 0.5f * M_PI;

  std::vector<hmap::VoronoiReturnType> types = {
      hmap::VoronoiReturnType::F1_SQUARED,
      hmap::VoronoiReturnType::F2_SQUARED,
      hmap::VoronoiReturnType::F1TF2_SQUARED,
      hmap::VoronoiReturnType::F1DF2_SQUARED,
      hmap::VoronoiReturnType::F2MF1_SQUARED,
      hmap::VoronoiReturnType::EDGE_DISTANCE_EXP,
      hmap::VoronoiReturnType::EDGE_DISTANCE_SQUARED,
      hmap::VoronoiReturnType::CONSTANT,
      hmap::VoronoiReturnType::CONSTANT_F2MF1_SQUARED};

  std::vector<hmap::Array> zs = {};

  for (auto type : types)
  {
    hmap::Array z = hmap::gpu::vorolines(shape,
                                         density,
                                         seed,
                                         k_smoothing,
                                         exp_sigma,
                                         alpha,
                                         alpha_span,
                                         type);
    hmap::remap(z);
    z = sqrt(z);
    zs.push_back(z);
  }

  hmap::export_banner_png("ex_vorolines.png", zs, hmap::Cmap::INFERNO);

  // FBM
  zs = {};

  for (auto type : types)
  {
    hmap::Array z = hmap::gpu::vorolines_fbm(shape,
                                             density,
                                             seed,
                                             k_smoothing,
                                             exp_sigma,
                                             alpha,
                                             alpha_span,
                                             type);
    hmap::remap(z);
    z = sqrt(z);
    zs.push_back(z);
  }

  hmap::export_banner_png("ex_vorolines_fbm.png", zs, hmap::Cmap::INFERNO);
}
