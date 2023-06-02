#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  const float            res = 4.f;
  const int              seed = 1;

  hmap::Array z = hmap::bump_field(shape, res, seed);
  z.to_png("ex_bump_field.png", hmap::cmap::viridis);
}
