#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  auto        z0 = z;

  // scale sediment talus with height
  auto  talus_layer = z;
  float talus = 0.5f / shape.x;
  hmap::remap(talus_layer, talus / 100.f, talus);

  // exclusion limit
  auto talus_upper_limit = hmap::Array(z.shape, 2.f / shape.x);
  int  iterations = 10;

  hmap::sediment_layer(z, talus_layer, talus_upper_limit, iterations, true);

  z.to_png_grayscale("out.png", CV_16U);

  hmap::export_banner_png("ex_sediment_layer.png",
                          {z0, z},
                          hmap::Cmap::TERRAIN,
                          true);
}
