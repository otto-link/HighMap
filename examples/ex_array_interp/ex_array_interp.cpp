#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z0 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  hmap::Vec2<int> new_shape = {1024, 1024};

  hmap::Array z1 = z0.resample_to_shape(new_shape);
  z1 = z1.resample_to_shape(shape);

  hmap::Array z2 = z0.resample_to_shape_bicubic(new_shape);
  z2.to_png("out.png", hmap::Cmap::JET);
  z2 = z2.resample_to_shape(shape);

  hmap::export_banner_png("ex_array_interp.png",
                          {z0, z1, z2},
                          hmap::Cmap::MAGMA);
}
