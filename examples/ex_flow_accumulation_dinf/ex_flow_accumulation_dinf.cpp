#include "highmap/array.hpp"
#include "highmap/hydrology.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  hmap::Vec2<int>   shape = {256, 256};
  hmap::Vec2<float> res = {2.f, 2.f};
  int               seed = 1;

  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::n_perlin, shape, res, seed);
  hmap::Array talus = hmap::gradient_talus(z);

  auto facc = hmap::flow_accumulation_dinf(z, talus.max());

  // very high values are less relevant
  hmap::clamp_max(facc, 100.f);

  z.to_png("ex_flow_accumulation_dinf0.png", hmap::cmap::terrain, true);
  facc.to_png("ex_flow_accumulation_dinf1.png", hmap::cmap::hot);
}
