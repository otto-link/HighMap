#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  std::vector<hmap::NoiseType> noise_list = {hmap::NoiseType::n_perlin,
                                             hmap::NoiseType::n_perlin_billow,
                                             hmap::NoiseType::n_perlin_half,
                                             hmap::NoiseType::n_simplex2,
                                             hmap::NoiseType::n_simplex2s,
                                             hmap::NoiseType::n_value,
                                             hmap::NoiseType::n_value_cubic,
                                             hmap::NoiseType::n_value_delaunay,
                                             hmap::NoiseType::n_value_linear,
                                             hmap::NoiseType::n_value_thinplate,
                                             hmap::NoiseType::n_worley,
                                             hmap::NoiseType::n_worley_double,
                                             hmap::NoiseType::n_worley_value};

  hmap::Array z = hmap::Array(hmap::Vec2<int>(0, shape.y));
  for (auto &noise_type : noise_list)
  {
    auto n = hmap::noise(noise_type, shape, kw, seed);
    hmap::remap(n);
    z = hstack(z, n);
  }

  z.to_png("ex_noise.png", hmap::cmap::terrain, true);
}
