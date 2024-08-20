#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  int             seed = 1;

  hmap::Vec4<float> bbox = {-1.f, 0.f, 0.5f, 1.5f};

  hmap::Cloud cloud = hmap::Cloud(10, seed, bbox);

  std::vector<float> x = cloud.get_x();
  std::vector<float> y = cloud.get_y();
  std::vector<float> values = cloud.get_values();

  // reference, pointwise values
  hmap::Array z0 = hmap::Array(shape);
  cloud.to_array(z0, bbox);

  // nearest
  hmap::Array z1 = hmap::interpolate2d(shape,
                                       x,
                                       y,
                                       values,
                                       hmap::InterpolationMethod2D::NEAREST,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       bbox);

  hmap::Array z2 = hmap::interpolate2d(shape,
                                       x,
                                       y,
                                       values,
                                       hmap::InterpolationMethod2D::DELAUNAY,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       bbox);

  z1.infos();

  // hmap::Array z0 = hmap::Array(shape);
  // cloud.to_array(z0, bbox);

  // hmap::Array z1 = hmap::Array(shape);
  // cloud.to_array_interp(z1, bbox);

  hmap::Array nx = 0.5f * hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                          shape,
                                          {2.f, 2.f},
                                          seed++);
  hmap::Array ny = 0.5f * hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                          shape,
                                          {2.f, 2.f},
                                          seed++);

  hmap::export_banner_png("ex_interpolate2d.png",
                          {z0, z1, z2},
                          hmap::Cmap::INFERNO);
}
