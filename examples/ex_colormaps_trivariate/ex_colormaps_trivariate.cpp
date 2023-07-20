#include <iostream>

#include "highmap.hpp"

int main(void)
{
  const std::vector<int>   shape = {512, 512};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);
  hmap::remap(z);
  z += 0.5f * hmap::step(shape, 30.f, 8.f / shape[0], &z, {-0.5f, 0.f});
  z *= hmap::gaussian_pulse(shape, 0.2f * shape[0]);
  hmap::gamma_correction(z, 0.7f);

  hmap::Array zb = hmap::minimum_local(z, 32);
  // hmap::hydraulic_ridge(z, 6.f / shape[0], 0.1f);
  hmap::hydraulic_stream(z, zb, 0.02f, 10.f / shape[0]);
  // hmap::hydraulic_vpipes(z);
  hmap::hydraulic_particle(z, 100000, seed);

  z.to_png("ex_colormaps_trivariate_hmap.png", hmap::cmap::gray);

  // trivariate

  hmap::Array dzx = hmap::gradient_x(z);
  hmap::Array dzy = hmap::gradient_y(z);

  std::vector<std::string> flist = {"amazon.bin",
                                    "china.bin",
                                    "desert.bin",
                                    "greenland.bin",
                                    "hawaii.bin",
                                    "himalaya.bin",
                                    "korea.bin",
                                    "rivers.bin",
                                    "sahara.bin",
                                    "snow.bin",
                                    "spain.bin",
                                    "stone.bin",
                                    "swamp.bin",
                                    "tundra.bin",
                                    "virgin.bin"};

  for (auto &fname : flist)
  {
    std::cout << fname << std::endl;
    hmap::Clut3D clut = hmap::Clut3D({32, 32, 32}, fname);
    auto         img = hmap::colorize_trivariate(z, dzx, dzy, clut, true);
    hmap::write_png_8bit("ex_colormaps_trivariate_" + fname + ".png",
                         img,
                         shape);
  }
}
