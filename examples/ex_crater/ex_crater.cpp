#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};

  float radius = 48.f;
  float sigma_outer = 32.f;
  float depth = 0.5f;

  // without noise
  hmap::Array z1 = hmap::crater(shape, radius, depth, sigma_outer);

  hmap::export_banner_png("ex_crater.png", {z1}, hmap::cmap::terrain, true);
  z1.to_file("out.bin");
}
