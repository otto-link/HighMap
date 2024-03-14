#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;
  hmap::Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f};

  hmap::PerlinFunction p = hmap::PerlinFunction(kw, seed);

  auto n = 0.4f * hmap::perlin(shape, kw, ++seed);

  auto z1 = hmap::Array(shape);
  auto z2 = hmap::Array(shape);

  fill_array_using_xy_function(z1, bbox, &n, &n, nullptr, p.get_function());

  int subsampling = 8;

  fill_array_using_xy_function(z2,
                               bbox,
                               &n,
                               &n,
                               nullptr,
                               p.get_function(),
                               subsampling);

  hmap::export_banner_png("ex_fill_array_using_xy_function.png",
                          {z1, z2},
                          hmap::cmap::jet);
}
