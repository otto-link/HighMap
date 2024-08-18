#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN_HALF,
                                  shape,
                                  kw,
                                  seed);

  hmap::Array k = hmap::curvature_gaussian(z);
  hmap::Array h = hmap::curvature_mean(z);

  int         ir = 16;
  hmap::Array si = hmap::shape_index(z, ir);

  hmap::Array un = hmap::unsphericity(z, ir);

  hmap::Array ac = hmap::accumulation_curvature(z, ir);

  z.to_png("ex_curvature_gaussian0.png", hmap::Cmap::TERRAIN);
  k.to_png("ex_curvature_gaussian1.png", hmap::Cmap::JET);
  h.to_png("ex_curvature_gaussian2.png", hmap::Cmap::JET);

  si.to_png("ex_curvature_gaussian3.png", hmap::Cmap::JET);
  un.to_png("ex_curvature_gaussian4.png", hmap::Cmap::JET);
  ac.to_png("ex_curvature_gaussian5.png", hmap::Cmap::JET);
}
