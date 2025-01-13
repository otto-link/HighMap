#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array noise = hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                      shape,
                                      kw,
                                      seed,
                                      8,
                                      0.f);
  hmap::remap(noise);

  // base path
  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path path = hmap::Path(4, seed, bbox.adjust(0.2f, -0.2f, 0.2f, -0.2f));
  path.reorder_nns();
  path.fractalize(8, seed);
  path.set_values_from_array(noise, bbox);

  auto zp = hmap::Array(shape);
  path.to_array(zp, bbox);

  hmap::Array dz = hmap::generate_riverbed(path, shape, bbox);
  hmap::remap(dz);

  dz.to_png_grayscale("out.png", CV_16U);
  dz.to_png("out1.png", hmap::Cmap::JET);

  hmap::export_banner_png("ex_generate_riverbed.png",
                          {zp, dz},
                          hmap::Cmap::INFERNO);
}
