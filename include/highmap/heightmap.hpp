/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

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
#include "highmap/io.hpp"
#include "highmap/vector.hpp"

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
  Vec2<float> shift;

  /**
   * @brief Scale of the tile in each direction, assuming the global domain is
   * a unit square.
   *
   * For example, if the tiling is {4, 2} without overlap, the scale is {0.25,
   * 0.5}.
   */
  Vec2<float> scale;

  /**
   * @brief Tile bounding box {xmin, xmax, ymin, ymax}.
   *
   */
  Vec4<float> bbox;

  /**
   * @brief Construct a new Tile object.
   *
   * @param shape Shape.
   * @param shift Shift.
   */
  Tile(Vec2<int> shape, Vec2<float> shift, Vec2<float> scale, Vec4<float> bbox);

  Tile(); /// @overload

  /**
   * @brief Assignment overloading (array).
   *
   * @param array
   */
  void operator=(const Array &array);

  /**
   * @brief Fill tile values by interpolating (bilinear) values from
   * another array.
   *
   * @param array Input array.
   */
  void from_array_interp(Array &array);

  /**
   * @brief Fill tile values by interpolating (nearest) values from
   * another array.
   *
   * @param array Input array.
   */
  void from_array_interp_nearest(Array &array);

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
  Vec2<int> shape;

  /**
   * @brief Heightmap bounding box {xmin, xmax, ymin, ymax}.
   *
   */
  Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f};

  /**
   * @brief Tiling setup (number of tiles in each direction).
   *
   */
  Vec2<int> tiling = {1, 1};

  /**
   * @brief Tile overlapping, in [0, 1[.
   *
   */
  float overlap = 0.f;

  /**
   * @brief Tile storage.
   *
   */
  std::vector<Tile> tiles = {};

  /**
   * @brief Construct a new HeightMap object.
   *
   * @param shape Shape.
   * @param bbox Bounding box.
   * @param tiling Tiling setup.
   * @param overlap Tile overlapping.
   */
  HeightMap(Vec2<int> shape, Vec4<float> bbox, Vec2<int> tiling, float overlap);

  HeightMap(Vec2<int> shape, Vec2<int> tiling,
            float overlap); /// @overload

  HeightMap(Vec2<int> shape, Vec4<float> bbox,
            Vec2<int> tiling); /// @overload

  HeightMap(Vec2<int> shape, Vec4<float> bbox); /// @overload

  HeightMap(Vec2<int> shape, Vec2<int> tiling); /// @overload

  HeightMap(Vec2<int> shape); /// @overload

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
   * @brief Set the tile overlapping.
   *
   * @param new_overlap New overlap.
   */
  void set_overlap(float new_overlap);

  /**
   * @brief Set the heightmap shape.
   *
   * @param new_shape New shape.
   */
  void set_shape(Vec2<int> new_shape);

  /**
   * @brief Set the shape / tiling / overlap in one pass.
   *
   * @param new_shape New shape.
   * @param new_tiling New tiling.
   * @param new_overlap New overlap.
   */
  void set_sto(Vec2<int> new_shape, Vec2<int> new_tiling, float new_overlap);

  /**
   * @brief Set the tiling setup.
   *
   * @param new_tiling New tiling.
   */
  void set_tiling(Vec2<int> new_tiling);

  /**
   * @brief Fill tile values by interpolating (bilinear) values from
   * another array.
   *
   * @param array Input array.
   *
   * **Example**
   * @include ex_heightmap_from_array.cpp
   *
   * **Result**
   * @image html ex_heightmap_from_array0.png
   * @image html ex_heightmap_from_array1.png
   */
  void from_array_interp(Array &array);

  /**
   * @brief Fill tile values by interpolating (nearest neighbor) values from
   * another array.
   *
   * @param array Input array.
   */
  void from_array_interp_nearest(Array &array);

  /**
   * @brief Print some informations about the object.
   *
   */
  void infos();

  /**
   * @brief Inverse the heightmap values (max - values).
   */
  void inverse();

  /**
   * @brief Return the value of the greatest element in the heightmap data.
   *
   * @return float
   */
  float max();

  /**
   * @brief Smooth the transitions between each tiles (when overlap > 0).
   *
   */
  void smooth_overlap_buffers();

  /**
   * @brief Return the value of the smallest element in the heightmap data.
   *
   * @return float
   */
  float min();

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
  Array to_array(Vec2<int> shape_export);

  Array to_array(); // @overload

  /**
   * @brief Returns the unique elements of the heightmap.
   *
   * @return std::vector<float> Unique values.
   */
  std::vector<float> unique_values();

  /**
   * @brief Update tile parameters.
   *
   */
  void update_tile_parameters();
};

struct HeightMapRGB
{
  std::vector<HeightMap> rgb;
  Vec2<int>              shape = {0, 0};

  HeightMapRGB();

  HeightMapRGB(HeightMap r, HeightMap g, HeightMap b);

  void set_sto(Vec2<int> new_shape, Vec2<int> new_tiling, float new_overlap);

  void colorize(HeightMap &h,
                float      vmin,
                float      vmax,
                int        cmap,
                bool       reverse = false);

  void colorize(HeightMap                      &h,
                float                           vmin,
                float                           vmax,
                std::vector<std::vector<float>> colormap_colors,
                bool                            reverse = false);

  void normalize();

  std::vector<uint8_t> to_img_8bit(Vec2<int> shape_img = {0, 0});

  void to_png_16bit(std::string fname);

  friend HeightMapRGB mix_heightmap_rgb(HeightMapRGB &rgb1,
                                        HeightMapRGB &rgb2,
                                        HeightMap    &t);

  friend HeightMapRGB mix_heightmap_rgb(HeightMapRGB &rgb1,
                                        HeightMapRGB &rgb2,
                                        float         t);

  friend HeightMapRGB mix_heightmap_rgb_ryb(HeightMapRGB &rgb1,
                                            HeightMapRGB &rgb2,
                                            HeightMap    &t);

  friend HeightMapRGB mix_heightmap_rgb_ryb(HeightMapRGB &rgb1,
                                            HeightMapRGB &rgb2,
                                            float         t);

  friend HeightMapRGB mix_heightmap_rgb_sqrt(HeightMapRGB &rgb1,
                                             HeightMapRGB &rgb2,
                                             HeightMap    &t);

  friend HeightMapRGB mix_heightmap_rgb_sqrt(HeightMapRGB &rgb1,
                                             HeightMapRGB &rgb2,
                                             float         t);
};

// shape, shift, scale, noise_x, noise_y

/**
 * @brief
 *
 * @param h
 * @param p_noise_x
 * @param p_noise_y
 * @param nullary_op
 *
 * **Example**
 * @include ex_heightmap_fill.cpp
 *
 * **Result**
 * @image html ex_heightmap_fill0.png
 * @image html ex_heightmap_fill1.png
 */
void fill(HeightMap                             &h,
          HeightMap                             *p_noise_x,
          HeightMap                             *p_noise_y,
          std::function<Array(Vec2<int>,
                              Vec2<float>,
                              Vec2<float>,
                              Array *p_noise_x,
                              Array *p_noise_y)> nullary_op);

void fill(HeightMap                          &h,
          HeightMap                          *p_noise_x,
          HeightMap                          *p_noise_y,
          HeightMap                          *p_stretching,
          std::function<Array(Vec2<int>,
                              Vec2<float>,
                              Vec2<float>,
                              hmap::Array *,
                              hmap::Array *,
                              hmap::Array *)> nullary_op);

// shape, shift, scale and noise
void fill(
    HeightMap &h,
    HeightMap *p_noise,
    std::function<Array(Vec2<int>, Vec2<float>, Vec2<float>, Array *p_noise)>
        nullary_op);

// shape, shift and scale
void fill(HeightMap                                                &h,
          std::function<Array(Vec2<int>, Vec2<float>, Vec2<float>)> nullary_op);

// shape only
void fill(HeightMap &h, std::function<Array(Vec2<int>)> nullary_op);

void transform(HeightMap &h, std::function<void(Array &)> unary_op);

void transform(HeightMap                                &h,
               std::function<void(Array &, Vec4<float>)> unary_op);

void transform(
    HeightMap                                                  &h,
    HeightMap                                                  *p_noise_x,
    HeightMap                                                  *p_noise_y,
    std::function<void(Array &, Vec4<float>, Array *, Array *)> unary_op);

void transform(HeightMap                                             &h,
               std::function<void(Array &, Vec2<float>, Vec2<float>)> unary_op);

// input array and mask
void transform(HeightMap                            &h,
               HeightMap                            *p_mask,
               std::function<void(Array &, Array *)> unary_op);

// for erosion
void transform(
    HeightMap       &h,
    hmap::HeightMap *p_1,
    hmap::HeightMap *p_2,
    hmap::HeightMap *p_3,
    hmap::HeightMap *p_4,
    hmap::HeightMap *p_5,
    std::function<void(Array &, Array *, Array *, Array *, Array *, Array *)>
        unary_op);

void transform(
    HeightMap                                              &h,
    hmap::HeightMap                                        *p_1,
    hmap::HeightMap                                        *p_2,
    hmap::HeightMap                                        *p_3,
    std::function<void(Array &, Array *, Array *, Array *)> unary_op);

void transform(HeightMap                                     &h,
               hmap::HeightMap                               *p_1,
               hmap::HeightMap                               *p_2,
               std::function<void(Array &, Array *, Array *)> unary_op);

void transform(HeightMap                            &h1,
               HeightMap                            &h2,
               std::function<void(Array &, Array &)> binary_op);

void transform(
    HeightMap                                                      &h1,
    HeightMap                                                      &h2,
    std::function<void(Array &, Array &, Vec2<float>, Vec2<float>)> binary_op);

void transform(HeightMap                                     &h1,
               HeightMap                                     &h2,
               HeightMap                                     &h3,
               std::function<void(Array &, Array &, Array &)> ternary_op);

// with returned array
void transform(HeightMap                    &h_out, // output
               HeightMap                    &h1,    // in 1
               std::function<Array(Array &)> unary_op);

void transform(HeightMap                             &h_out, // output
               HeightMap                             &h1,    // in 1
               HeightMap                             &h2,    // in 2
               std::function<Array(Array &, Array &)> binary_op);

void transform(
    HeightMap                                              &h1,
    HeightMap                                              &h2,
    HeightMap                                              &h3,
    HeightMap                                              &h4,
    std::function<void(Array &, Array &, Array &, Array &)> ternary_op);

} // namespace hmap
