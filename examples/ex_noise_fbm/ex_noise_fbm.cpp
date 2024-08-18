#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  std::vector<hmap::NoiseType> noise_list = {
      hmap::NoiseType::PERLIN,
      hmap::NoiseType::PERLIN_BILLOW,
      hmap::NoiseType::PERLIN_HALF,
      hmap::NoiseType::SIMPLEX2,
      hmap::NoiseType::SIMPLEX2S,
      hmap::NoiseType::VALUE,
      hmap::NoiseType::VALUE_CUBIC,
      // --- too slow, do not use ---
      // hmap::NoiseType::n_value_delaunay,
      // hmap::NoiseType::n_value_linear,
      // hmap::NoiseType::n_value_thinplate,
      hmap::NoiseType::WORLEY,
      hmap::NoiseType::WORLEY_DOUBLE,
      hmap::NoiseType::WORLEY_VALUE};

  auto ctrl_array = hmap::noise(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::remap(ctrl_array);

  // fbm
  {
    hmap::Array z = hmap::Array(hmap::Vec2<int>(0, shape.y));
    for (auto &noise_type : noise_list)
    {
      auto n = hmap::noise_fbm(noise_type,
                               shape,
                               kw,
                               seed,
                               8,
                               0.7f,
                               0.5f,
                               2.f,
                               &ctrl_array);
      hmap::remap(n);
      z = hstack(z, n);
    }
    z.to_png("ex_noise_fbm0.png", hmap::Cmap::TERRAIN, true);
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
    z.to_png("ex_noise_fbm1.png", hmap::Cmap::TERRAIN, true);
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
    z.to_png("ex_noise_fbm2.png", hmap::Cmap::TERRAIN, true);
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
    z.to_png("ex_noise_fbm3.png", hmap::Cmap::TERRAIN, true);
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
    z.to_png("ex_noise_fbm4.png", hmap::Cmap::TERRAIN, true);
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
    z.to_png("ex_noise_fbm5.png", hmap::Cmap::TERRAIN, true);
  }

  // Parberry
  {
    auto n = hmap::noise_parberry(shape, kw, seed);
    hmap::remap(n);
    n.to_png("ex_noise_fbm6.png", hmap::Cmap::TERRAIN, true);
  }
}
