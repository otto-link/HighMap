#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 0;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::SIMPLEX2, shape, kw, seed);
  hmap::remap(z);
  auto z0 = z;

  // --- 2-step erosion at different scales

  hmap::hydraulic_procedural(z, ++seed, 1.f / 8.f, 0.2f);
  auto z1 = z;
  hmap::hydraulic_procedural(z, ++seed, 1.f / 32.f, 0.2f / 8.f);

  hmap::export_banner_png("ex_hydraulic_procedural0.png",
                          {z0, z1, z},
                          hmap::Cmap::TERRAIN,
                          true);

  // --- all profiles

  std::vector<hmap::ErosionProfile> profiles = {
      hmap::ErosionProfile::COSINE,
      hmap::ErosionProfile::SAW_SHARP,
      hmap::ErosionProfile::SAW_SMOOTH,
      hmap::ErosionProfile::SHARP_VALLEYS,
      hmap::ErosionProfile::SQUARE_SMOOTH,
      hmap::ErosionProfile::TRIANGLE_GRENIER,
      hmap::ErosionProfile::TRIANGLE_SHARP,
      hmap::ErosionProfile::TRIANGLE_SMOOTH,
  };

  std::vector<hmap::Array> stack = {z0};
  for (auto ep : profiles)
  {
    hmap::Array ze = z0;
    hmap::hydraulic_procedural(ze, seed, 1.f / 8.f, 0.2f, ep);
    stack.push_back(ze);
  }

  hmap::export_banner_png("ex_hydraulic_procedural1.png",
                          stack,
                          hmap::Cmap::TERRAIN,
                          true);

  z1.to_png_grayscale("hmap.png", CV_16U);
}
