#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  std::vector<hmap::NoiseType> noise_list = {
      hmap::NoiseType::n_perlin,
      hmap::NoiseType::n_perlin_billow,
      hmap::NoiseType::n_perlin_half,
      hmap::NoiseType::n_simplex2,
      hmap::NoiseType::n_simplex2s,
      hmap::NoiseType::n_value,
      hmap::NoiseType::n_value_cubic,
      // --- too slow, do not use ---
      // hmap::NoiseType::n_value_delaunay,
      // hmap::NoiseType::n_value_linear,
      // hmap::NoiseType::n_value_thinplate,
      hmap::NoiseType::n_worley,
      hmap::NoiseType::n_worley_double,
      hmap::NoiseType::n_worley_value};

  auto b = hmap::perlin(shape, kw, seed + 1);

  // fbm
  {
    hmap::Array z = hmap::Array(hmap::Vec2<int>(0, shape.y));
    for (auto &noise_type : noise_list)
    {
      auto n =
          hmap::noise_fbm(noise_type, shape, kw, seed, 8, 0.7f, 0.5f, 2.f, &b);
      hmap::remap(n);
      z = hstack(z, n);
    }
    z.to_png("ex_noise_fbm0.png", hmap::cmap::terrain, true);
  }

  // iq
  {
    hmap::Array z = hmap::Array(hmap::Vec2<int>(0, shape.y));
    for (auto &noise_type : noise_list)
    {
      auto n = hmap::noise_iq(noise_type, shape, kw, seed);
      hmap::remap(n);
      z = hstack(z, n);
    }
    z.to_png("ex_noise_fbm1.png", hmap::cmap::terrain, true);
  }

  // jordan
  {
    hmap::Array z = hmap::Array(hmap::Vec2<int>(0, shape.y));
    for (auto &noise_type : noise_list)
    {
      auto n = hmap::noise_jordan(noise_type, shape, kw, seed);
      hmap::remap(n);
      z = hstack(z, n);
    }
    z.to_png("ex_noise_fbm2.png", hmap::cmap::terrain, true);
  }

  // pingpong
  {
    hmap::Array z = hmap::Array(hmap::Vec2<int>(0, shape.y));
    for (auto &noise_type : noise_list)
    {
      auto n = hmap::noise_pingpong(noise_type, shape, kw, seed);
      hmap::remap(n);
      z = hstack(z, n);
    }
    z.to_png("ex_noise_fbm3.png", hmap::cmap::terrain, true);
  }

  // ridged
  {
    hmap::Array z = hmap::Array(hmap::Vec2<int>(0, shape.y));
    for (auto &noise_type : noise_list)
    {
      auto n = hmap::noise_ridged(noise_type, shape, kw, seed);
      hmap::remap(n);
      z = hstack(z, n);
    }
    z.to_png("ex_noise_fbm4.png", hmap::cmap::terrain, true);
  }

  // swiss
  {
    hmap::Array z = hmap::Array(hmap::Vec2<int>(0, shape.y));
    for (auto &noise_type : noise_list)
    {
      auto n = hmap::noise_swiss(noise_type, shape, kw, seed);
      hmap::remap(n);
      z = hstack(z, n);
    }
    z.to_png("ex_noise_fbm5.png", hmap::cmap::terrain, true);
  }
}
