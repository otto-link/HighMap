#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           kw = 4.f;
  int             seed = 1;

  hmap::Array noise = hmap::fbm_perlin(shape, {kw, kw}, seed);
  hmap::remap(noise, -0.05f, 0.05f);

  float decay = 1.f;

  hmap::Array z1 =
      hmap::worley_polyline(shape, kw, seed, decay, &noise, &noise);

  int   octaves = 8;
  float weight = 0.7f;
  float persistence = 0.5f;
  float lacunarity = 2.f;

  hmap::Array z2 = hmap::fbm_worley_polyline(shape,
                                             kw,
                                             seed,
                                             decay,
                                             octaves,
                                             weight,
                                             persistence,
                                             lacunarity,
                                             &noise,
                                             &noise);

  hmap::export_banner_png("ex_worley_polyline.png",
                          {z1, z2},
                          hmap::cmap::inferno);
}
