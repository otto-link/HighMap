#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  // shape = {1024, 1024};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  std::vector<int> noise_list = {hmap::noise_type::noise_simplex2,
                                 hmap::noise_type::noise_simplex2s,
                                 hmap::noise_type::noise_worley,
                                 hmap::noise_type::noise_perlin,
                                 hmap::noise_type::noise_value_cubic,
                                 hmap::noise_type::noise_value,
                                 hmap::noise_type::noise_perlin_billow,
                                 hmap::noise_type::noise_perlin_cliff};

  std::vector<int> fractal_list = {hmap::fractal_type::fractal_none,
                                   hmap::fractal_type::fractal_fbm,
                                   hmap::fractal_type::fractal_ridged,
                                   hmap::fractal_type::fractal_pingpong};

  // noises

  hmap::Array z1 = hmap::Array(hmap::Vec2<int>(0, shape.y));
  for (auto &noise_type : noise_list)
  {
    auto n = hmap::fbm(shape,
                       res,
                       seed,
                       noise_type,
                       hmap::fractal_type::fractal_fbm);
    hmap::remap(n);
    z1 = hstack(z1, n);
  }

  // fractals

  hmap::Array z2 = hmap::Array(hmap::Vec2<int>(0, shape.y));
  for (auto &fractal_type : fractal_list)
  {
    auto n = hmap::fbm(shape,
                       res,
                       seed,
                       hmap::noise_type::noise_simplex2,
                       fractal_type);
    hmap::remap(n);
    z2 = hstack(z2, n);
  }

  auto z3 = hmap::fbm(shape,
                      res,
                      seed,
                      hmap::noise_type::noise_perlin_cliff,
                      hmap::fractal_type::fractal_fbm);

  // hmap::export_wavefront_obj("hmap.obj", z3);

  z1.to_png("ex_fbm0.png", hmap::cmap::terrain, true);
  z2.to_png("ex_fbm1.png", hmap::cmap::terrain, true);
}
