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
  hmap::Array chc = hmap::curvature_horizontal_cross_sectional(z, ir);
  hmap::Array chp = hmap::curvature_horizontal_plan(z, ir);
  hmap::Array cht = hmap::curvature_horizontal_tangential(z, ir);
  hmap::Array cri = hmap::curvature_ring(z, ir);
  hmap::Array cro = hmap::curvature_rotor(z, ir);
  hmap::Array cvl = hmap::curvature_vertical_longitudinal(z, ir);
  hmap::Array cvp = hmap::curvature_vertical_profile(z, ir);
  hmap::Array si = hmap::shape_index(z, ir);
  hmap::Array un = hmap::unsphericity(z, ir);

  // output
  std::vector<hmap::Array> alist =
      {z, k, h, ac, chc, chp, cht, cri, cro, cvl, cvp, si, un};

  for (auto &a : alist)
    hmap::remap(a);

  hmap::export_banner_png("ex_curvature.png", alist, hmap::Cmap::JET);
}
