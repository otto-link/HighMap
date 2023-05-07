/**
 * @file array.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <vector>

class Array
{
public:
  std::vector<int>   shape;
  std::vector<float> vector;

  Array(std::vector<int> shape);
};
