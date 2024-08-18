#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 6;
  int             npoints = 10;

  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path        path = hmap::Path(npoints, seed, {1.3f, 1.7f, -0.2, 0.2f});
  path.reorder_nns();

  auto z1 = hmap::Array(shape);
  path.to_array(z1, bbox);

  auto z2 = hmap::Array(shape);
  {
    hmap::Path path_c = path;

    float ratio = 0.2f;

    path_c.meanderize(ratio);
    path_c.to_array(z2, bbox);
  }

  auto z3 = hmap::Array(shape);
  {
    hmap::Path path_c = path;

    float ratio = 0.4f;
    float noise_ratio = 0.1f;
    int   iterations = 2;

    path_c.meanderize(ratio, noise_ratio, seed, iterations);
    path_c.to_array(z3, bbox);
  }

  hmap::export_banner_png("ex_path_meanderize.png",
                          {z1, z2, z3},
                          hmap::Cmap::INFERNO);
}
