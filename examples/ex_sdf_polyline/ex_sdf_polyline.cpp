#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {1024, 1024};
  hmap::Vec2<float> kw = {32.f, 32.f};
  int               seed = 1;

  hmap::Array noise = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::remap(noise, 0.f, 1.f);

  // base path
  hmap::Vec4<float> bbox = {1.f, 2.f, -0.5f, 0.5f};
  hmap::Path path = hmap::Path(10, seed, bbox.adjust(0.2f, -0.2f, 0.2f, -0.2f));
  path.reorder_nns();
  path.set_values_from_array(noise, bbox);

  auto zp = hmap::Array(shape);
  path.to_array(zp, bbox);

  hmap::Array sdf_2d = hmap::sdf_2d_polyline(path, shape, bbox);
  hmap::smooth_cpulse(sdf_2d, 8);
  hmap::remap(sdf_2d);

  hmap::export_banner_png("ex_sdf_polyline.png",
                          {zp, sdf_2d},
                          hmap::Cmap::INFERNO);
}
