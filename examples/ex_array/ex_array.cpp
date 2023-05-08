#include <iostream>

#include "highmap/array.hpp"

int main(void)
{
  const std::vector<int> shape = {6, 4};
  hmap::Array            a = hmap::Array(shape);

  a = 1.f;

  for (int i = 0; i < a.shape[0]; i++)
    for (int j = 0; j < a.shape[1]; j++)
      std::cout << i << " " << j << " " << a(i, j) << std::endl;
}
