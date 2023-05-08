#include <iostream>

#include "highmap/array.hpp"

int main(void)
{
  const std::vector<int> shape = {6, 4};
  hmap::Array            a = hmap::Array(shape);

  for (auto &v : a.get_vector())
    std::cout << v << std::endl;
}
