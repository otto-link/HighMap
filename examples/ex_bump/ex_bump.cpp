#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};

  hmap::Array z1 = hmap::bump(shape);

  // with control array
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;
  hmap::Array       ctrl_array = hmap::noise(hmap::NoiseType::PERLIN,
                                       shape,
                                       kw,
                                       seed);
  hmap::remap(ctrl_array, 0.8f, 1.2f);

  hmap::Array z2 = hmap::bump(shape, 1.f, &ctrl_array);

  hmap::export_banner_png("ex_bump.png", {z1, z2}, hmap::Cmap::INFERNO, false);
}
