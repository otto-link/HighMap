#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  std::vector<hmap::NoiseType> noise_list = {hmap::NoiseType::PERLIN,
                                             hmap::NoiseType::PERLIN_BILLOW,
                                             hmap::NoiseType::PERLIN_HALF,
                                             hmap::NoiseType::SIMPLEX2,
                                             hmap::NoiseType::SIMPLEX2S,
                                             hmap::NoiseType::VALUE,
                                             hmap::NoiseType::VALUE_CUBIC,
                                             hmap::NoiseType::VALUE_DELAUNAY,
                                             hmap::NoiseType::VALUE_LINEAR,
                                             hmap::NoiseType::WORLEY,
                                             hmap::NoiseType::WORLEY_DOUBLE,
                                             hmap::NoiseType::WORLEY_VALUE};

  hmap::Array z = hmap::Array(hmap::Vec2<int>(0, shape.y));
  for (auto &noise_type : noise_list)
  {
    auto n = hmap::noise(noise_type, shape, kw, seed);
    hmap::remap(n);
    z = hstack(z, n);
  }

  z.to_png("ex_noise.png", hmap::Cmap::TERRAIN, true);
}
