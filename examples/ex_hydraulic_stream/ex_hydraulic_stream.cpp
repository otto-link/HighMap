#include "highmap/array.hpp"
#include "highmap/erosion.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  auto        z0 = z;

  float       c_erosion = 0.1f;
  float       talus_ref = 5.f / (float)shape.x;
  int         iradius = 64;
  hmap::Array z_bedrock = hmap::minimum_local(z, iradius);

  auto z1 = z;
  hmap::hydraulic_stream(z1, c_erosion, talus_ref);

  auto z2 = z;
  int  ir = 5;

  auto erosion_map = hmap::Array(shape);
  auto deposition_map = hmap::Array(shape);

  hmap::hydraulic_stream(z2,
                         c_erosion,
                         talus_ref,
                         &z_bedrock,
                         &erosion_map,
                         &deposition_map,
                         ir);

  hmap::export_banner_png("ex_hydraulic_stream0.png",
                          {z0, z1, z2},
                          hmap::cmap::terrain,
                          true);

  hmap::export_banner_png("ex_hydraulic_stream1.png",
                          {erosion_map, deposition_map},
                          hmap::cmap::inferno);
}
