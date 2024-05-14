#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  float azimuth = 180.f; // exposure from the west
  float zenith = 45.f;   // "sun elevation angle"

  hmap::Array ts = hmap::topographic_shading(z, azimuth, zenith);
  hmap::Array hs = hmap::hillshade(z, azimuth, zenith);

  ts.to_png("ex_hillshade0.png", hmap::cmap::bone);
  hs.to_png("ex_hillshade1.png", hmap::cmap::bone);
}
