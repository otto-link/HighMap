#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           kw = 2.f;
  uint            seed = 0;

  hmap::Array angle(shape, 0.f);

  std::vector<hmap::PhasorProfile> profiles = {
      hmap::PhasorProfile::COSINE_BULKY,
      hmap::PhasorProfile::COSINE_PEAKY,
      hmap::PhasorProfile::COSINE_STD,
      hmap::PhasorProfile::COSINE_SQUARE,
      hmap::PhasorProfile::TRIANGLE,
  };

  std::vector<hmap::Array> arrays;

  for (auto p : profiles)
  {
    hmap::Array z = hmap::phasor(p, shape, kw, angle, seed);
    arrays.push_back(z);
  }

  // fbm phasor
  hmap::Array z = hmap::phasor_fbm(hmap::PhasorProfile::COSINE_BULKY,
                                   shape,
                                   kw,
                                   angle,
                                   seed);
  arrays.push_back(z);

  z.to_png_grayscale("out.png", CV_16U);

  hmap::export_banner_png("ex_phasor.png", arrays, hmap::Cmap::TERRAIN);
}
