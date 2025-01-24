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
  hmap::remap(noise, 0.f, 1.f);

  // base path
  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path path = hmap::Path(9, seed, bbox.adjust(0.2f, -0.2f, 0.2f, -0.2f));
  path.reorder_nns();
  // path.fractalize(2, seed);
  path.set_values_from_array(noise, bbox);

  auto zp = hmap::Array(shape);
  path.to_array(zp, bbox);

  hmap::Array sdf_2d = hmap::sdf_2d_polyline(path, shape, bbox);
  hmap::remap(sdf_2d);

  hmap::Array sdf_2d_bezier = hmap::sdf_2d_polyline_bezier(path, shape, bbox);
  hmap::remap(sdf_2d_bezier);

  hmap::export_banner_png("ex_sdf_polyline.png",
                          {zp, sdf_2d, sdf_2d_bezier},
                          hmap::Cmap::INFERNO);
}
