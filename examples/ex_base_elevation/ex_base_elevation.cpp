#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  // 2x3 grid (column ordering, Fortran style)
  std::vector<std::vector<float>> values = {{0.f, 0.5f, 0.f}, {1.f, 1.5f, 2.f}};

  hmap::Array z1 = hmap::base_elevation(shape, values);

  hmap::Array nx = 0.4f * hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                          shape,
                                          {2.f, 2.f},
                                          1);
  hmap::Array ny = 0.4f * hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                          shape,
                                          {2.f, 2.f},
                                          2);
  float       width_factor = 1.f;
  hmap::Array z2 = hmap::base_elevation(shape, values, width_factor, &nx, &ny);

  hmap::export_banner_png("ex_base_elevation.png",
                          {z1, z2},
                          hmap::Cmap::INFERNO);
}
