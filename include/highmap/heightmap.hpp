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
  /**
   * @brief Tile shift in each direction, assuming the global domain is a unit
   * square.
   *
   * For example, if the tiling is {4, 2}, the shift of tile {3, 2} is {0.75,
   * 0.5}.
   */
  std::vector<float> shift;

  /**
   * @brief Tile bounding box {xmin, xmax, ymin, ymax}.
   *
   */
  std::vector<float> bbox;

  /**
   * @brief Construct a new Tile object.
   *
   * @param shape Shape.
   * @param shift Shift.
   */
  Tile(std::vector<int> shape, std::vector<float> shift);

  Tile(); /// @overload

  /**
   * @brief Assignment overloading (array).
   *
   * @param array
   */
  void operator=(const Array &array);

  /**
   * @brief Print some informations about the object.
   *
   */
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
  /**
   * @brief Heightmap global shape.
   *
   */
  std::vector<int> shape;

  /**
   * @brief Heightmap bounding box {xmin, xmax, ymin, ymax}.
   *
   */
  std::vector<float> bbox = {0.f, 1.f, 0.f, 1.f};

  /**
   * @brief Tiling setup (number of tiles in each direction).
   *
   */
  std::vector<int> tiling = {1, 1};

  /**
   * @brief Scale of each tile in each direction, assuming the global domain is
   * a unit square.
   *
   * For example, if the tiling is {4, 2}, the scale is {0.25, 0.5}.
   */
  std::vector<float> tile_scale = {1.f, 1.f};

  /**
   * @brief Total number of tiles.
   *
   */
  int ntiles;

  /**
   * @brief Tile storage.
   *
   */
  std::vector<Tile> tiles = {};

  /**
   * @brief Shape of individual tile.
   *
   */
  std::vector<int> shape_tile = {};

  /**
   * @brief Construct a new HeightMap object.
   *
   * @param shape Shape.
   * @param bbox Bounding box.
   * @param tiling Tiling setup.
   */
  HeightMap(std::vector<int>   shape,
            std::vector<float> bbox,
            std::vector<int>   tiling);

  HeightMap(std::vector<int> shape, std::vector<float> bbox); /// @overload

  HeightMap(std::vector<int> shape, std::vector<int> tiling); /// @overload

  HeightMap(std::vector<int> shape); /// @overload

  HeightMap(); /// @overload

  //----------------------------------------
  // accessors
  //----------------------------------------

  //----------------------------------------
  // methods
  //----------------------------------------

  /**
   * @brief Get the number of tiles
   *
   * @return size_t Number of tiles.
   */
  size_t get_ntiles();

  /**
   * @brief Get the tile linear index.
   *
   * @param i Tile i index.
   * @param j Tile j index
   * @return int Linear index.
   */
  int get_tile_index(int i, int j);

  /**
   * @brief Set the heightmap shape.
   *
   * @param new_shape New shape.
   */
  void set_shape(std::vector<int> new_shape);

  /**
   * @brief Set the tiling setup.
   *
   * @param new_tiling New tiling.
   */
  void set_tiling(std::vector<int> new_tiling);

  /**
   * @brief Print some informations about the object.
   *
   */
  void infos();

  /**
   * @brief Return the value of the smallest element in the heightmap data.
   *
   * @return float
   */
  float min();

  /**
   * @brief Return the value of the greatest element in the heightmap data.
   *
   * @return float
   */
  float max();

  /**
   * @brief Remap heightmap elements from a starting range to a target range.
   *
   * By default the starting range is taken to be [min(), max()] of the input
   * array.
   *
   * @param array Input array.
   * @param vmin The lower bound of the range to remap to.
   * @param vmax The lower bound of the range to remap to.
   * @param from_min The lower bound of the range to remap from.
   * @param from_max The upper bound of the range to remap from.
   */
  void remap(float vmin = 0.f, float vmax = 1.f);

  void remap(float vmin, float vmax, float from_min, float from_max);

  /**
   * @brief Return the heightmap as an array.
   *
   * @param shape_export Array shape.
   * @return Array Resulting array.
   */
  Array to_array(std::vector<int> shape_export);

  Array to_array(); // @overload

  /**
   * @brief Update tile parameters.
   *
   */
  void update_tile_parameters();

private:
};

void fill(
    HeightMap &h,
    HeightMap &noise,
    std::function<Array(std::vector<int>, std::vector<float>, Array *p_noise)>
        nullary_op); // shape and shift and noise

void fill(HeightMap &h,
          std::function<Array(std::vector<int>, std::vector<float>)>
              nullary_op); // shape and shift

void fill(HeightMap                             &h,
          std::function<Array(std::vector<int>)> nullary_op); // shape only

void transform(HeightMap &h, std::function<void(Array &)> unary_op);

void transform(HeightMap                            &h1,
               HeightMap                            &h2,
               std::function<void(Array &, Array &)> binary_op);

void transform(HeightMap                                     &h1,
               HeightMap                                     &h2,
               HeightMap                                     &h3,
               std::function<void(Array &, Array &, Array &)> ternary_op);

} // namespace hmap
