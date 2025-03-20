#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  shape = {1024, 1024};
  hmap::Vec2<float> kw = {2.f, 2.f};
  uint              seed = 1;

  hmap::Array       z = hmap::Array(shape);
  hmap::Vec4<float> bbox = hmap::unit_square_bbox();

  float mu = 1.02f;

  std::unique_ptr<hmap::NoiseFunction> p = std::unique_ptr<hmap::NoiseFunction>(
      new hmap::ParberryFunction(kw, seed, mu));

  hmap::FbmFunction f = hmap::FbmFunction(std::move(p), 8, 0.7f, 0.5f, 2.f);

  fill_array_using_xy_function(z,
                               bbox,
                               nullptr,
                               nullptr,
                               nullptr,
                               nullptr,
                               f.get_delegate());

  z.to_png("out.png", hmap::Cmap::TERRAIN, true);
  z.infos();
}
