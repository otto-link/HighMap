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
  std::vector<float> bbox;

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
  std::vector<int>   tiling = {1, 1};
  int                ntiles;
  std::vector<Tile>  tiles = {};
  std::vector<int>   shape_tile = {};

  HeightMap(std::vector<int> shape, std::vector<float> bbox);

  HeightMap(std::vector<int> shape, std::vector<int> tiling);

  HeightMap(std::vector<int> shape); // @overload

  //----------------------------------------
  // accessors
  //----------------------------------------

  //----------------------------------------
  // methods
  //----------------------------------------

  size_t get_ntiles();

  int get_tile_index(int i, int j);

  /**
   * @brief Print some informations about the object.
   *
   */
  void infos() const;

  float min();

  float max();

  /**
   * @brief Remap heightmap values.
   *
   * @param vmin The lower bound of the range to remap to.
   * @param vmax The lower bound of the range to remap to.
   */
  void remap(float vmin = 0.f, float vmax = 1.f);

  /**
   * @brief Rescale a pair of wavenumbers according to the heightmap tiling
   * configuration.
   *
   * @param kw Global wavenumbers (for the whole heightmap)
   * @return std::vector<float> Wavenumbers for the tile.
   */
  std::vector<float> rescale_kw(std::vector<float> kw);

  Array to_array();

  Array to_array(std::vector<int> shape_export); // @overload

  void update_tile_parameters();

private:
};

void fill(HeightMap &h,
          std::function<Array(std::vector<int>, std::vector<float>)>
              nullary_op); // shape and shift

void fill(HeightMap                             &h,
          std::function<Array(std::vector<int>)> nullary_op); // shape only

void transform(HeightMap &h, std::function<void(Array &)> unary_op);

void transform(HeightMap                            &h1,
               HeightMap                            &h2,
               std::function<void(Array &, Array &)> binary_op);

void transform(HeightMap                            &h1,
               HeightMap                            &h2,
               HeightMap                            &h3,
               std::function<void(Array &, Array &)> binary_op);

} // namespace hmap