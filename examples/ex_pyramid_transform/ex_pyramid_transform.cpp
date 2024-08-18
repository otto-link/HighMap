#include <iostream>

#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  shape = {512, 512};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);
  hmap::remap(z);

  int                        nlevels = 4;
  hmap::PyramidDecomposition pyr = hmap::PyramidDecomposition(z, nlevels);

  pyr.decompose();

  // 'transform' function
  auto fct = [&seed](const hmap::Array &input, const int current_level)
  {
    std::cout << "applying erosion to level and shape: " << current_level << " "
              << input.shape.x << " " << input.shape.y << "\n";

    // apply hydraulic erosion to each component
    hmap::Array output = input;
    float       particle_density = 0.4f;
    int         nparticles = (int)(particle_density * input.size());
    hydraulic_particle(output, nparticles, ++seed);

    return output;
  };

  auto zr = pyr.transform(fct, hmap::pyramid_transform_support::FULL);

  hmap::export_banner_png("ex_pyramid_transform.png",
                          {z, zr},
                          hmap::Cmap::TERRAIN,
                          true);
}
