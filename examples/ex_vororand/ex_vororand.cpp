#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int> shape = {256, 256};
  float           density = 8.f;
  float           variability = 4.f;
  int             seed = 1;
  float           k_smoothing = 0.05f;
  float           exp_sigma = 0.01f;

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
    hmap::Array z = hmap::gpu::vororand(shape,
                                        density,
                                        variability,
                                        seed,
                                        k_smoothing,
                                        exp_sigma,
                                        type);
    hmap::remap(z);
    zs.push_back(z);
  }

  hmap::export_banner_png("ex_vororand.png", zs, hmap::Cmap::INFERNO);
}
