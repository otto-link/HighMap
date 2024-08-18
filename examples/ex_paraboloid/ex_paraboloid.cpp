#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {256, 256};
  float           angle = 15.f;

  auto z = hmap::paraboloid(shape, angle, 1.f, 0.5f);

  z.to_png("ex_paraboloid.png", hmap::Cmap::INFERNO);
}
