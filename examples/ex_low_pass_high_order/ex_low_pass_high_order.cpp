#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int>   shape = {256, 256};
  const std::vector<float> res = {2.f, 2.f};
  int                      seed = 1;

  hmap::Array z = hmap::fbm_perlin(shape, res, seed);

  auto zf = z;
  int  order = 7;
  hmap::low_pass_high_order(zf, order);

  hmap::export_banner_png("ex_low_pass_high_order.png",
                          {z, zf},
                          hmap::cmap::viridis);
}
