#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           angle = 30.f;
  float           talus = 4.f;
  float           width = 0.1f;

  bool sharp_bottom = false;

  float kw = 4.f;
  uint  seed = 1;
  auto  noise = 0.2f * hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                      shape,
                                       {kw, kw},
                                      seed,
                                      8,
                                      0.f);

  auto z1 = hmap::rift(shape, angle, talus, width);
  auto z2 =
      hmap::rift(shape, angle, talus, width, sharp_bottom, nullptr, &noise);

  // with control array
  hmap::Array ctrl_array = hmap::noise(hmap::NoiseType::PERLIN,
                                       shape,
                                       {kw, kw},
                                       seed);
  hmap::remap(ctrl_array, 0.f, 1.f);

  hmap::Array z3 = hmap::rift(shape,
                              angle,
                              talus,
                              width,
                              true, // sharp bottom
                              &ctrl_array);

  hmap::export_banner_png("ex_rift.png", {z1, z2, z3}, hmap::Cmap::INFERNO);
}
