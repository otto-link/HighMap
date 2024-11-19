#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  shape = {512, 512};
  // shape = {1024, 1024};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 0;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::SIMPLEX2, shape, kw, seed);
  hmap::remap(z);
  auto z0 = z;

  hmap::ErosionProfile ep = hmap::ErosionProfile::TRIANGLE_SMOOTH;

  hmap::hydraulic_procedural(z, ++seed, 1.f / 8.f, 0.2f, ep);
  auto z1 = z;

  hmap::hydraulic_procedural(z, ++seed, 1.f / 32.f, 0.2f / 8.f, ep);

  hmap::export_banner_png("ex_hydraulic_procedural.png",
                          {z0, z1, z},
                          hmap::Cmap::TERRAIN,
                          true);

  z.to_png_grayscale("hmap.png", CV_16U);
}
