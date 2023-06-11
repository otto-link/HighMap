#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  float                  angle = 30.f;
  float                  talus = 4.f / shape[0];

  float kw = 4.f;
  uint  seed = 1;
  auto  noise = 0.2f * hmap::fbm_perlin(shape, {kw, kw}, seed, 8, 0.f);

  auto z1 = hmap::step(shape, angle, talus);
  auto z2 = hmap::step(shape, angle, talus, &noise);

  hmap::export_banner_png("ex_step.png", {z1, z2}, hmap::cmap::viridis);
}
