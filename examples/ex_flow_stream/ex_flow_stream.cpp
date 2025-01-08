#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::remap(z);

  hmap::Vec2<int> ij_start(100, 128);
  hmap::Path      path = hmap::flow_stream(z, ij_start);

  hmap::Vec4<float> bbox(0.f, 1.f, 0.f, 1.f);

  hmap::Array zv(shape);
  path.to_array(zv, bbox);

  hmap::Array zd = z;

  float riverbank_talus = 1.f / shape.x;
  int   merging_ir = 4;
  float riverbed_talus = 0.2 / shape.x;
  hmap::dig_river(zd, path, riverbank_talus, merging_ir, riverbed_talus);

  hmap::export_banner_png("ex_flow_stream.png",
                          {z, zv, zd},
                          hmap::Cmap::TERRAIN,
                          true);
}
