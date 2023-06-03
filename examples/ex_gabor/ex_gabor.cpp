#include "highmap/array.hpp"
#include "highmap/io.hpp"
#include "highmap/primitives.hpp"

int main(void)
{
  const std::vector<int> shape = {256, 256};
  float                  kw = 15.f;
  float                  angle = 30.f;

  hmap::Array z = hmap::gabor(shape, kw, angle);
  z.to_png("ex_gabor.png", hmap::cmap::viridis);
}
