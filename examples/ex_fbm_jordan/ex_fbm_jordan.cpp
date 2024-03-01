#include "highmap.hpp"
#include "highmap/dbg.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  std::vector<int> noise_list = {hmap::noise_type::noise_simplex2,
                                 hmap::noise_type::noise_simplex2s,
                                 hmap::noise_type::noise_worley,
                                 hmap::noise_type::noise_perlin,
                                 hmap::noise_type::noise_value_cubic,
                                 hmap::noise_type::noise_value};

  // noises

  hmap::Array z1 = hmap::Array(hmap::Vec2<int>(0, shape.y));
  for (auto &noise_type : noise_list)
  {
    auto n = hmap::fbm_jordan(shape, res, seed, noise_type);
    hmap::remap(n);
    z1 = hstack(z1, n);
  }

  z1.to_png("ex_fbm_jordan.png", hmap::cmap::terrain, true);
}
