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

  int ir = 4;

  hmap::Array ac = hmap::accumulation_curvature(z, ir);
  hmap::Array si = hmap::shape_index(z, ir);
  hmap::Array un = hmap::unsphericity(z, ir);

  // output
  std::vector<hmap::Array> alist = {z, k, h, ac, si, un};

  for (auto &a : alist)
    hmap::remap(a);

  hmap::export_banner_png("ex_curvature.png", alist, hmap::Cmap::JET);
}
