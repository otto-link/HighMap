/**
 * @file heightmap.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-07-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "highmap/array.hpp"

namespace hmap
{

/**
 * @brief HeightMap class, to manipulate heightmap (with contextual
 * informations).
 *
 */
class HeightMap
{
public:
  std::vector<int>   shape;
  std::vector<float> bbox = {0.f, 1.f, 0.f, 1.f};
  std::vector<float> xy0 = {0.f, 0.f};

  HeightMap(std::vector<int> shape, std::vector<float> bbox);

  HeightMap(std::vector<int> shape); // @overload

  //----------------------------------------
  // accessors
  //----------------------------------------

  

  //----------------------------------------
  // methods
  //----------------------------------------

private:
  Array array = Array({0, 0});
};

} // namespace hmap