#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  // shape = {1024, 1024};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);
  auto z0 = z;

  float       c_erosion = 0.1f;
  float       talus_ref = 5.f / (float)shape.x;
  int         iradius = 64;
  hmap::Array z_bedrock = hmap::minimum_local(z, iradius);

  auto z1 = z;
  hmap::hydraulic_stream(z1, c_erosion, talus_ref);

  auto z2 = z;
  int  ir = 5;

  auto erosion_map = hmap::Array(shape);
  auto moisture_map = z;

  hmap::hydraulic_stream(z2,
                         c_erosion,
                         talus_ref,
                         &z_bedrock,
                         &moisture_map,
                         &erosion_map,
                         ir);

  hmap::gpu::init_opencl();

  // log scale
  auto z3 = z;
  int  deposition_ir = 32;
  c_erosion = 0.2f;
  hmap::hydraulic_stream_log(z3, c_erosion, talus_ref, deposition_ir);

  z3.dump();

  hmap::export_banner_png("ex_hydraulic_stream0.png",
                          {z0, z1, z2, z3},
                          hmap::Cmap::TERRAIN,
                          true);

  erosion_map.to_png("ex_hydraulic_stream1.png", hmap::Cmap::INFERNO);
}
