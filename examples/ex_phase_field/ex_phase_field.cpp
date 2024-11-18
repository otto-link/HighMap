#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 0;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::SIMPLEX2, shape, kw, seed);
  hmap::remap(z);

  float kp = 4.f;
  int   width = 64;

  float density = 4.f;
  bool  rotate90 = true;

  hmap::Array phi0 = hmap::phase_field(z, kp, width, ++seed, -1, density);
  hmap::Array phi1 =
      hmap::phase_field(z, kp, width, ++seed, -1, density, rotate90);

  hmap::remap(phi0);
  hmap::remap(phi1);

  hmap::export_banner_png("ex_phase_field.png",
                          {z, phi0, phi1},
                          hmap::Cmap::JET);
}
