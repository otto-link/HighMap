#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {4.f, 4.f};
  int               seed = 1;

  hmap::Array z1 = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  hmap::Tensor nmap = hmap::normal_map(z1);

  // rebuild input elevation field from normal map
  hmap::Array z2 = hmap::normal_map_to_heightmap(nmap);

  hmap::remap(z1);
  hmap::remap(z2);

  hmap::export_banner_png("ex_normal_map_to_heightmap.png",
                          {z1, z2},
                          hmap::Cmap::JET);
}
