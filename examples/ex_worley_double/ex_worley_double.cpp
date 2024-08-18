#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  float ratio = 0.5f;
  float k = 0.1f;

  // ratio multiplier
  hmap::Array ctrl_array = hmap::noise(hmap::NoiseType::PERLIN,
                                       shape,
                                       kw,
                                       seed++);
  hmap::remap(ctrl_array);

  hmap::Array z = hmap::worley_double(shape, kw, seed, ratio, k, &ctrl_array);

  z.to_png("ex_worley_double.png", hmap::Cmap::TERRAIN, true);
}
