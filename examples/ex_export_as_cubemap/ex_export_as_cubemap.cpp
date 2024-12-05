#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {1024, 1024};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 0;

  hmap::Array z = hmap::noise_ridged(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);
  hmap::hydraulic_stream(z, &z, 0.05f, 4.f / shape.x);

  hmap::export_as_cubemap("ex_export_as_cubemap.png", z);
}
