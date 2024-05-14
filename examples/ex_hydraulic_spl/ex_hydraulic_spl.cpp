#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  float talus_ref = 10.f / (float)shape.x;
  float c_erosion = 0.01f;
  int   iterations = 10;

  int         iradius = 64;
  hmap::Array z_bedrock = hmap::minimum_local(z, iradius);

  auto z1 = z;
  hmap::hydraulic_spl(z1, c_erosion, talus_ref, iterations, &z_bedrock);

  auto z2 = z;
  auto erosion_map = hmap::Array(shape);
  auto moisture_map = z;
  hmap::hydraulic_spl(z2,
                      c_erosion,
                      talus_ref,
                      iterations,
                      &z_bedrock,
                      &moisture_map,
                      &erosion_map);

  hmap::export_banner_png("ex_hydraulic_spl0.png",
                          {z, z1, z2},
                          hmap::cmap::terrain,
                          true);

  erosion_map.to_png("ex_hydraulic_spl1.png", hmap::cmap::inferno);
}
