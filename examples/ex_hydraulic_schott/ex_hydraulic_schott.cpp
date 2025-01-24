#include "highmap.hpp"

int main(void)
{
  hmap::gpu::init_opencl();

  hmap::Vec2<int> shape = {256, 256};
  shape = {512, 512};
  hmap::Vec2<float> kw = {2.f, 2.f};
  int               seed = 2;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, kw, seed);
  hmap::remap(z);
  auto z0 = z;

  int         iterations = 400;
  hmap::Array talus(shape, 2.f / (float)shape.x); // for thermal

  hmap::gpu::hydraulic_schott(z, iterations, talus);

  hmap::export_banner_png("ex_hydraulic_schott.png",
                          {z0, z},
                          hmap::Cmap::TERRAIN,
                          true);
}
