#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 2;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::remap(z);
  auto z0 = z;

  int   iterations = 1;
  int   sub_iterations_erosion = 60;
  int   sub_iterations_thermal = 20;
  float talus = 4.f / (float)shape.x; // for thermal

  hmap::Array flow_map_output(shape, 1.f);

  for (int it = 0; it < iterations; it++)
  {
    hmap::hydraulic_schott(z,
                           sub_iterations_erosion,
                           0.5f, // 0.5 times deposition iterations
                           0.3f, // erosion
                           0.5f, // deposition
                           nullptr,
                           &flow_map_output);

    hmap::thermal_schott(z, talus, sub_iterations_thermal);
  }

  // rescale output and generate png
  hmap::remap(z, z0.min(), z0.max());
  hmap::remap(flow_map_output);

  hmap::export_banner_png("ex_hydraulic_schott.png",
                          {z0, z, flow_map_output},
                          hmap::Cmap::TERRAIN,
                          true);
}
