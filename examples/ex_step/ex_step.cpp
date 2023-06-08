#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  float                  angle = 30.f;
  float                  talus = 4.f / shape[0];

  auto z1 = hmap::step(shape, angle, talus);

  auto z2 = z1;
  hmap::warp_fbm(z2, 16.f, {4.f, 4.f}, (uint)1);

  z1.to_file("out.bin");

  hmap::export_banner_png("ex_step.png", {z1, z2}, hmap::cmap::viridis);
}
