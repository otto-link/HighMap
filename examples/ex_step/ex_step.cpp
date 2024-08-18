#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           angle = 30.f;
  float           talus = 4.f;

  float kw = 4.f;
  uint  seed = 1;
  auto  noise = 0.2f * hmap::noise_fbm(hmap::NoiseType::PERLIN,
                                      shape,
                                       {kw, kw},
                                      seed,
                                      8,
                                      0.f);

  auto z1 = hmap::step(shape, angle, talus);
  auto z2 = hmap::step(shape, angle, talus, nullptr, &noise);

  // with control array
  hmap::Array ctrl_array = hmap::noise(hmap::NoiseType::PERLIN,
                                       shape,
                                       {kw, kw},
                                       seed);
  hmap::remap(ctrl_array, 0.8f, 1.2f);

  hmap::Array z3 = hmap::step(shape, angle, talus, &ctrl_array);

  hmap::export_banner_png("ex_step.png", {z1, z2, z3}, hmap::Cmap::INFERNO);
}
