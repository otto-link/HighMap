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
    float gradient_scale = 0.05f;

    auto n = hmap::fbm_iq(shape, res, seed, noise_type, gradient_scale);
    hmap::remap(n);
    z1 = hstack(z1, n);
  }

  z1.to_png("ex_fbm_iq.png", hmap::cmap::terrain, true);
}
