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

  hmap::Array zd1 = z;

  float       riverbank_talus = 1.f / shape.x;
  int         river_width = 1;
  int         merging_width = 4;
  float       depth = 0.01f;
  float       riverbed_talus = 0.2 / shape.x;
  float       noise_ratio = 0.9f;
  hmap::Array mask;

  hmap::dig_river(zd1,
                  {path},
                  riverbank_talus,
                  river_width,
                  merging_width,
                  depth,
                  riverbed_talus,
                  noise_ratio,
                  seed,
                  &mask);

  // multiple streams
  hmap::Vec2<int> ij_start2(128, 180);
  hmap::Path      path2 = hmap::flow_stream(z, ij_start2);

  hmap::Array zd2 = z;
  hmap::dig_river(zd2,
                  {path, path2},
                  riverbank_talus,
                  river_width,
                  merging_width,
                  depth,
                  riverbed_talus,
                  noise_ratio,
                  seed,
                  &mask);

  zd2.to_png_grayscale("out.png", CV_16U);

  hmap::export_banner_png("ex_flow_stream.png",
                          {z, zv, zd1, zd2, mask},
                          hmap::Cmap::JET,
                          true);
}
