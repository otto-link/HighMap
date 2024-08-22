#include <iostream>

#include "highmap.hpp"

int main(void)
{
  hmap::Vec2<int> shape = {6, 4};
  hmap::Array     a = hmap::Array(shape);

  a = 1.f;

  for (int i = 0; i < a.shape.x; i++)
    for (int j = 0; j < a.shape.y; j++)
      std::cout << i << " " << j << " " << a(i, j) << std::endl;
}
