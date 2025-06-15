#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z1 = hmap::gpu::voronoi_edge_distance(shape, kw, seed);

  hmap::Array noise = 0.2f * hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                             shape,
                                             {4.f, 4.f},
                                             seed + 1);

  hmap::Vec2<float> jitter = {0.5f, 0.5f};
  hmap::Array       z2 = hmap::gpu::voronoi_edge_distance(shape,
                                                    kw,
                                                    seed,
                                                    jitter,
                                                    nullptr,
                                                    &noise);

  hmap::export_banner_png("ex_voronoi_edge_distance.png",
                          {z1, z2},
                          hmap::Cmap::INFERNO);
}
