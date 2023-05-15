#include <iostream>

#include "highmap/array.hpp"
#include "highmap/colorize.hpp"
#include "highmap/io.hpp"
#include "highmap/noise.hpp"
#include "highmap/op.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {4.f, 4.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  float azimuth = 180.f; // exposure from the west
  float zenith = 45.f;   // "sun elevation angle"

  hmap::Array ts = hmap::topographic_shading(z, azimuth, zenith);
  hmap::Array hs = hmap::hillshade(z, azimuth, zenith);

  ts.to_png("ex_hillshade0.png", hmap::cmap::gray);
  hs.to_png("ex_hillshade1.png", hmap::cmap::gray);
}
