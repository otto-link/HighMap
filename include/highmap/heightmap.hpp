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
#include <functional>

#include "highmap/array.hpp"

namespace hmap
{

class Tile : public Array
{
public:
  std::vector<float> shift;

  std::vector<float> bbox; // TODO
  std::vector<float> xy0;  // TODO

  Tile();

  Tile(std::vector<int> shape, std::vector<float> shift);

  void operator=(const Array &array);

  void infos() const;
};

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
  std::vector<float> xy0 = {0.5f, 0.f};
  std::vector<int>   tiling = {1, 1};

  std::vector<Tile> tiles = {};
  std::vector<int>  shape_tile = {};

  HeightMap(std::vector<int> shape, std::vector<float> bbox);

  HeightMap(std::vector<int> shape, std::vector<int> tiling);

  HeightMap(std::vector<int> shape); // @overload

  //----------------------------------------
  // accessors
  //----------------------------------------

  //----------------------------------------
  // methods
  //----------------------------------------

  int get_tile_index(int i, int j);

  /**
   * @brief Print some informations about the object.
   *
   */
  void infos() const;

  Array to_array();

  void update_tile_parameters();

private:
};

void fill(
    HeightMap                                                 &h,
    std::function<Array(std::vector<int>, std::vector<float>)> nullary_op);

void transform1(HeightMap &h, std::function<void(Array &)> unary_op);

} // namespace hmap