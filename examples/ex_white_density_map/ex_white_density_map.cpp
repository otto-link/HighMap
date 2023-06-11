#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  int                    seed = 1;

  hmap::Array dmap = hmap::fbm_perlin(shape, {2.f, 2.f}, seed);
  hmap::remap(dmap);

  hmap::Array w = hmap::white_density_map(0.5f * dmap, seed);

  hmap::export_banner_png("ex_white_density_map.png",
                          {dmap, w},
                          hmap::cmap::viridis);
}