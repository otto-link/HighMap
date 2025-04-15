/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file heightmap.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
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
#include "highmap/export.hpp"

namespace hmap
{

enum NormalMapBlendingMethod : int
{
  NMAP_LINEAR,
  NMAP_DERIVATIVE,
  NMAP_UDN,
  NMAP_UNITY,
  NMAP_WHITEOUT
};

static std::map<std::string, int> normal_map_blending_method_as_string = {
    {"Linear", NMAP_LINEAR},
    {"Partial derivative", NMAP_DERIVATIVE},
    {"Unreal Developer Network", NMAP_UDN},
    {"Unity", NMAP_UNITY},
    {"Whiteout", NMAP_WHITEOUT},
};

enum TransformMode : int
{
  DISTRIBUTED,  ///< Distributed across multiple processors or threads.
  SEQUENTIAL,   ///< Performed sequentially in a single thread.
  SINGLE_ARRAY, ///< Transformation is applied to a single array of data.
};

static std::map<std::string, int> transform_mode_as_string = {
    {"Distributed", DISTRIBUTED},
    {"Sequential", SEQUENTIAL},
    {"Single array", SINGLE_ARRAY},
};

// --- forward declarations
class HeightmapRGBA;
HeightmapRGBA mix_heightmap_rgba(HeightmapRGBA &rgba1,
                                 HeightmapRGBA &rgba2,
                                 bool           use_sqrt_avg = true);
HeightmapRGBA mix_heightmap_rgba(std::vector<HeightmapRGBA *> p_rgba_list,
                                 bool use_sqrt_avg = true);

// --- classes

/**
 * @brief Tile class, to manipulate a restricted region of an heightmap (with
 * contextual informations).
 */
class Tile : public Array
{
public:
  /**
   * @brief Tile shift in each direction, assuming the global domain is a unit
   * square. For example, if the tiling is {4, 2}, the shift of tile {3, 2} is
   * {0.75, 0.5}.
   */
  Vec2<float> shift;

  /**
   * @brief Scale of the tile in each direction, assuming the global domain is a
   * unit square. For example, if the tiling is {4, 2} without overlap, the
   * scale is {0.25, 0.5}.
   */
  Vec2<float> scale;

  /**
   * @brief Tile bounding box {xmin, xmax, ymin, ymax}.
   */
  Vec4<float> bbox;

  /**
   * @brief Construct a new Tile object.
   *
   * @param shape Shape.
   * @param shift Shift.
   */
  Tile(Vec2<int> shape, Vec2<float> shift, Vec2<float> scale, Vec4<float> bbox);

  Tile(); ///< @overload

  /**
   * @brief Assignment overloading (array).
   *
   * @param array
   */
  void operator=(const Array &array);

  /**
   * @brief Fill tile values by interpolating (bilinear) values from another
   * array.
   *
   * @param array Input array.
   */
  void from_array_interp(Array &array);

  /**
   * @brief Fill tile values by interpolating (bicubic) values from another
   * array.
   *
   * @param array Input array.
   */
  void from_array_interp_bicubic(Array &array);

  /**
   * @brief Fill tile values by interpolating (bilinear) values from another
   * array.
   *
   * @param array Input array.
   */
  void from_array_interp_bilinear(Array &array);

  /**
   * @brief Fill tile values by interpolating (nearest) values from another
   * array.
   *
   * @param array Input array.
   */
  void from_array_interp_nearest(Array &array);

  /**
   * @brief Print some informations about the object.
   */
  void infos() const;
};

/**
 * @brief HeightMap class, to manipulate heightmap (with contextual
 * informations).
 */
class Heightmap
{
public:
  /**
   * @brief Heightmap global shape.
   */
  Vec2<int> shape;

  /**
   * @brief Tiling setup (number of tiles in each direction).
   */
  Vec2<int> tiling = {1, 1};

  /**
   * @brief Tile overlapping, in [0, 1[.
   */
  float overlap = 0.f;

  /**
   * @brief Tile storage.
   */
  std::vector<Tile> tiles = {};

  Heightmap(Vec2<int> shape, Vec2<int> tiling,
            float overlap); ///< @overload

  Heightmap(Vec2<int> shape,
            Vec2<int> tiling,
            float     overlap,
            float     fill_value); ///< @overload

  Heightmap(Vec2<int> shape, Vec2<int> tiling); ///< @overload

  Heightmap(Vec2<int> shape); ///< @overload

  Heightmap(); ///< @overload

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
  size_t get_ntiles() const;

  /**
   * @brief Get the tile linear index.
   *
   * @param  i Tile i index.
   * @param  j Tile j index
   * @return   int Linear index.
   */
  int get_tile_index(int i, int j) const;

  float get_value_bilinear(float x, float y) const;

  float get_value_nearest(float x, float y) const;

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
   * @param new_shape   New shape.
   * @param new_tiling  New tiling.
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
   * @brief Fill tile values by interpolating (bilinear) values from another
   * array.
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
   * @brief Fill tile values by interpolating (bicubic) values from another
   * array.
   *
   * @param array Input array.
   */
  void from_array_interp_bicubic(Array &array);

  /**
   * @brief Fill tile values by interpolating (bilinear) values from another
   * array.
   *
   * @param array Input array.
   */
  void from_array_interp_bilinear(Array &array);

  /**
   * @brief Fill tile values by interpolating (nearest neighbor) values from
   * another array.
   *
   * @param array Input array.
   */
  void from_array_interp_nearest(Array &array);

  /**
   * @brief Print some informations about the object.
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
   * @brief Return the mean of the heightmap data.
   *
   * @return float
   */
  float mean();

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
   * @param array    Input array.
   * @param vmin     The lower bound of the range to remap to.
   * @param vmax     The lower bound of the range to remap to.
   * @param from_min The lower bound of the range to remap from.
   * @param from_max The upper bound of the range to remap from.
   */
  void remap(float vmin = 0.f, float vmax = 1.f);

  void remap(float vmin,
             float vmax,
             float from_min,
             float from_max); ///< @overload

  /**
   * @brief Smooth the transitions between each tiles (when overlap > 0).
   */
  void smooth_overlap_buffers();

  /**
   * @brief Return the sum of the heightmap data.
   *
   * @return float
   */
  float sum();

  /**
   * @brief Return the heightmap as an array.
   *
   * @param  shape_export Array shape.
   * @return              Array Resulting array.
   */
  Array to_array(Vec2<int> shape_export);

  Array to_array(); ///< @overload

  /**
   * @brief Converts the heightmap to a 16-bit grayscale representation.
   *
   * @return A `std::vector<uint16_t>` containing the 16-bit grayscale image
   * data.
   */
  std::vector<uint16_t> to_grayscale_image_16bit();

  std::vector<uint16_t> to_grayscale_image_16bit_multithread();

  std::vector<uint8_t> to_grayscale_image_8bit();

  /**
   * @brief Returns the unique elements of the heightmap.
   *
   * @return std::vector<float> Unique values.
   */
  std::vector<float> unique_values();

  /**
   * @brief Update tile parameters.
   */
  void update_tile_parameters();
};

/**
 * @brief HeightMap class, to manipulate a set of RGB heightmap for heightmap
 * texturing.
 */
struct HeightmapRGB
{
  /**
   * @brief RGB component heightmap storage.
   *
   * **Example**
   * @include ex_ex_heightmap_rgb.cpp
   *
   * **Result**
   * @image html ex_ex_heightmap_rgb0.png
   * @image html ex_ex_heightmap_rgb1.png
   * @image html ex_ex_heightmap_rgb2.png
   * @image html ex_ex_heightmap_rgb3.png
   */
  std::vector<Heightmap> rgb;

  /**
   * @brief Shape.
   */
  Vec2<int> shape = {0, 0};

  /**
   * @brief Constructor.
   * @param r Heightmap for R (red) component.
   * @param g Heightmap for G (green) component.
   * @param b Heightmap for B (blue) component.
   */
  HeightmapRGB(Heightmap r, Heightmap g, Heightmap b);

  HeightmapRGB(); ///< @overload

  /**
   * @brief Set the shape / tiling / overlap in one pass.
   *
   * @param new_shape   New shape.
   * @param new_tiling  New tiling.
   * @param new_overlap New overlap.
   */
  void set_sto(Vec2<int> new_shape, Vec2<int> new_tiling, float new_overlap);

  /**
   * @brief Fill RGB heightmap components based on a colormap and an input
   * reference heightmap.
   * @param h       Input heightmap.
   * @param vmin    Lower bound for scaling to array [0, 1].
   * @param vmax    Upper bound for scaling to array [0, 1]
   * @param cmap    Colormap (see {@link cmap}).
   * @param reverse Reverse colormap.
   */
  void colorize(Heightmap &h,
                float      vmin,
                float      vmax,
                int        cmap,
                bool       reverse = false);

  /**
   * @brief Fill RGB heightmap components based on a colormap and an input
   * reference heightmap.
   * @param h               Input heightmap.
   * @param vmin            Lower bound for scaling to array [0, 1].
   * @param vmax            Upper bound for scaling to array [0, 1]
   * @param colormap_colors Colormap RGB colors as a vector of RGB colors.
   * @param reverse         Reverse colormap.
   */
  void colorize(Heightmap                      &h,
                float                           vmin,
                float                           vmax,
                std::vector<std::vector<float>> colormap_colors,
                bool                            reverse = false);

  /**
   * @brief Normalize RGB heightmaps amplitude.
   */
  void normalize();

  /**
   * @brief Convert the RGB heightmap to a 8bit RGB image.
   * @param  shape_img Resulting image shape.
   * @return           Image data.
   */
  std::vector<uint8_t> to_img_8bit(Vec2<int> shape_img = {0, 0});

  /**
   * @brief Export the RGB heightmap to a 16bit png file.
   * @param fname File name.
   */
  void to_png(const std::string &fname, int depth = CV_8U);

  /**
   * @brief Mix two RGB heightmap using linear interpolation.
   * @param  rgb1 1st RGB heightmap.
   * @param  rgb2 2st RGB heightmap.
   * @param  t    Mixing parameter, in [0, 1].
   * @return      RGB heightmap.
   */
  friend HeightmapRGB mix_heightmap_rgb(HeightmapRGB &rgb1,
                                        HeightmapRGB &rgb2,
                                        Heightmap    &t);

  friend HeightmapRGB mix_heightmap_rgb(HeightmapRGB &rgb1,
                                        HeightmapRGB &rgb2,
                                        float         t); ///< @overload

  /**
   * @brief Mix two RGB heightmap using weighted quadratic averaging.
   * @param  rgb1 1st RGB heightmap.
   * @param  rgb2 2st RGB heightmap.
   * @param  t    Mixing parameter, in [0, 1].
   * @return      RGB heightmap.
   */
  friend HeightmapRGB mix_heightmap_rgb_sqrt(HeightmapRGB &rgb1,
                                             HeightmapRGB &rgb2,
                                             Heightmap    &t);

  friend HeightmapRGB mix_heightmap_rgb_sqrt(HeightmapRGB &rgb1,
                                             HeightmapRGB &rgb2,
                                             float         t);
};

/**
 * @brief HeightMap class, to manipulate a set of RGBA heightmap for heightmap
 * texturing.
 *
 * **Example**
 * @include ex_ex_heightmap_rgba.cpp
 *
 * **Result**
 * @image html ex_ex_heightmap_rgba0.png
 * @image html ex_ex_heightmap_rgba1.png
 * @image html ex_ex_heightmap_rgba2.png
 * @image html ex_ex_heightmap_rgba3.png
 * @image html ex_ex_heightmap_rgba4.png
 */
struct HeightmapRGBA
{
  /**
   * @brief RGBA component heightmap storage.
   */
  std::vector<Heightmap> rgba;

  /**
   * @brief Shape.
   */
  Vec2<int> shape = {0, 0};

  /**
   * @brief Constructor.
   * @param r Heightmap for R (red) component.
   * @param g Heightmap for G (green) component.
   * @param b Heightmap for B (blue) component.
   * @param a Heightmap for A (alpha) component.
   */
  HeightmapRGBA(Heightmap r, Heightmap g, Heightmap b, Heightmap a);

  HeightmapRGBA(Vec2<int> shape,
                Vec2<int> tiling,
                float     overlap,
                Array     array_r,
                Array     array_g,
                Array     array_b,
                Array     array_a);

  HeightmapRGBA(Vec2<int> shape, Vec2<int> tiling, float overlap);

  HeightmapRGBA(); ///< @overload

  /**
   * @brief Set the alpha channel.
   * @param new_alpha Alpha value(s).
   */
  void set_alpha(Heightmap new_alpha);

  void set_alpha(float new_alpha);

  /**
   * @brief Set the shape / tiling / overlap in one pass.
   *
   * @param new_shape   New shape.
   * @param new_tiling  New tiling.
   * @param new_overlap New overlap.
   */
  void set_sto(Vec2<int> new_shape, Vec2<int> new_tiling, float new_overlap);

  /**
   * @brief Export the RGB heightmap to a 8bit png file.
   * @param fname File name.
   */
  void to_png(const std::string &fname, int depth = CV_8U);

  /**
   * @brief Fill RGB heightmap components based on a colormap and an input
   * reference heightmap.
   * @param color_level Input heightmap for color level.
   * @param vmin        Lower bound for scaling to array [0, 1].
   * @param vmax        Upper bound for scaling to array [0, 1]
   * @param cmap        Colormap (see {@link cmap}).
   * @param p_alpha     Reference to input heightmap for alpha channel,
   *                    expected in [0, 1].
   * @param reverse     Reverse colormap.
   */
  void colorize(Heightmap &color_level,
                float      vmin,
                float      vmax,
                int        cmap,
                Heightmap *p_alpha = nullptr,
                bool       reverse = false,
                Heightmap *p_noise = nullptr);

  /**
   * @brief Fill RGBA heightmap components based on a colormap and input
   * reference heightmaps for the color level and the transparency.
   * @param color_level     Input heightmap for color level.
   * @param vmin            Lower bound for scaling to array [0, 1].
   * @param vmax            Upper bound for scaling to array [0, 1]
   * @param colormap_colors Colormap RGB colors as a vector of RGB colors.
   * @param p_alpha         Reference to input heightmap for alpha
   *                        channel, expected in [0, 1].
   * @param reverse         Reverse colormap.
   */
  void colorize(Heightmap                      &color_level,
                float                           vmin,
                float                           vmax,
                std::vector<std::vector<float>> colormap_colors,
                Heightmap                      *p_alpha = nullptr,
                bool                            reverse = false,
                Heightmap                      *p_noise = nullptr);

  /**
   * @brief Computes the luminance of an RGBA height map.
   *
   * This method creates a grayscale `HeightMap` based on the luminance
   * values calculated from the red, green, and blue channels of the RGBA
   * height map. The luminance is computed using the standard formula: \f$
   * L =
   * 0.299 \times R + 0.587 \times G + 0.114 \times B \f$.
   *
   * @return A `HeightMap` representing the grayscale luminance of the
   *         current RGBA height map.
   *
   * @see    https://stackoverflow.com/questions/596216 for details on the
   *         luminance calculation.
   */
  Heightmap luminance();

  /**
   * @brief Mix two RGBA heightmap using alpha compositing ("over").
   * @param  rgba1        1st RGBA heightmap.
   * @param  rgba2        2st RGBA heightmap.
   * @param  use_sqrt_avg Whether to use or not square averaging.
   * @return              RGBA heightmap.
   */
  friend HeightmapRGBA mix_heightmap_rgba(HeightmapRGBA &rgba1,
                                          HeightmapRGBA &rgba2,
                                          bool           use_sqrt_avg);

  /**
   * @brief Mix two RGBA heightmap using alpha compositing ("over").
   * @param  rgba_plist   Heightmap reference list.
   * @param  use_sqrt_avg Whether to use or not square averaging.
   * @return              RGBA heightmap.
   */
  friend HeightmapRGBA mix_heightmap_rgba(
      std::vector<HeightmapRGBA *> rgba_plist,
      bool                         use_sqrt_avg);

  /**
   * @brief Normalize RGBA heightmaps amplitude.
   */
  void normalize();

  /**
   * @brief Convert the RGB heightmap to a 8bit RGB image.
   * @param  shape_img Resulting image shape.
   * @return           Image data.
   */
  std::vector<uint8_t> to_img_8bit(Vec2<int> shape_img = {0, 0});
};

/**
 * @brief Mixes two normal maps in RGBA format to create a blended normal map.
 *
 * This function blends a base normal map and a detail normal map using a
 * specified blending method. The detail map can be scaled to control the
 * intensity of its effect.
 *
 * @param  nmap_base       Reference to the base normal map in RGBA format.
 * @param  nmap_detail     Reference to the detail normal map in RGBA format.
 * @param  detail_scaling  Scaling factor for the detail normal map intensity in
 *                         [-1.f, 1.f]. Default is 1.0f.
 * @param  blending_method Method to blend the two normal maps. Options are
 *                         specified by the NormalMapBlendingMethod enum (e.g.,
 *                         NMAP_DERIVATIVE).
 *
 * @return                 A HeightMapRGBA object that contains the result of
 *                         blending the base and detail normal maps.
 *
 * **Example**
 * @include ex_mix_normal_map_rgba.cpp
 *
 * **Result**
 * @image html ex_mix_normal_map_rgba0.png
 * @image html ex_mix_normal_map_rgba1.png
 * @image html ex_mix_normal_map_rgba2.png
 * @image html ex_mix_normal_map_rgba3.png
 * @image html ex_mix_normal_map_rgba4.png
 * @image html ex_mix_normal_map_rgba5.png
 * @image html ex_mix_normal_map_rgba6.png
 */
HeightmapRGBA mix_normal_map_rgba(HeightmapRGBA          &nmap_base,
                                  HeightmapRGBA          &nmap_detail,
                                  float                   detail_scaling = 1.f,
                                  NormalMapBlendingMethod blending_method =
                                      NormalMapBlendingMethod::NMAP_DERIVATIVE);

/**
 * @brief Fills the heightmap using the provided noise maps and operation.
 *
 * This function fills the heightmap `h` using the noise maps `p_noise_x` and
 * `p_noise_y` and the provided `nullary_op` function. The `nullary_op` function
 * is expected to take a `Vec2<int>` and `Vec4<float>` as input and return an
 * `Array` using the noise maps.
 *
 * @param h          The heightmap to be filled.
 * @param p_noise_x  Pointer to the noise map for the x-axis.
 * @param p_noise_y  Pointer to the noise map for the y-axis.
 * @param nullary_op The operation to be applied for filling the heightmap.
 *
 * **Example**
 * @include ex_heightmap_fill.cpp
 *
 * **Result**
 * @image html ex_heightmap_fill0.png
 * @image html ex_heightmap_fill1.png
 */
[[deprecated]] void fill(
    Heightmap &h,
    Heightmap *p_noise_x,
    Heightmap *p_noise_y,
    std::function<
        Array(Vec2<int>, Vec4<float>, Array *p_noise_x, Array *p_noise_y)>
        nullary_op);

[[deprecated]] void fill(Heightmap                          &h,
                         Heightmap                          &hin,
                         Heightmap                          *p_noise_x,
                         Heightmap                          *p_noise_y,
                         std::function<Array(hmap::Array &,
                                             Vec2<int>,
                                             Vec4<float>,
                                             hmap::Array *,
                                             hmap::Array *)> nullary_op);

[[deprecated]] void fill(Heightmap                          &h,
                         Heightmap                          *p_noise_x,
                         Heightmap                          *p_noise_y,
                         Heightmap                          *p_stretching,
                         std::function<Array(Vec2<int>,
                                             Vec4<float>,
                                             hmap::Array *,
                                             hmap::Array *,
                                             hmap::Array *)> nullary_op);

// shape, shift, scale and noise
[[deprecated]] void fill(
    Heightmap                                                   &h,
    Heightmap                                                   *p_noise,
    std::function<Array(Vec2<int>, Vec4<float>, Array *p_noise)> nullary_op);

// shape, shift and scale
[[deprecated]] void fill(
    Heightmap                                   &h,
    std::function<Array(Vec2<int>, Vec4<float>)> nullary_op);

// shape only
[[deprecated]] void fill(Heightmap                      &h,
                         std::function<Array(Vec2<int>)> nullary_op);

[[deprecated]] void transform(Heightmap                   &h,
                              std::function<void(Array &)> unary_op);

[[deprecated]] void transform(
    Heightmap                                &h,
    std::function<void(Array &, Vec4<float>)> unary_op);

[[deprecated]] void transform(
    Heightmap                                         &h,
    Heightmap                                         *p_noise_x,
    std::function<void(Array &, Vec4<float>, Array *)> unary_op);

[[deprecated]] void transform(
    Heightmap                                                  &h,
    Heightmap                                                  *p_noise_x,
    Heightmap                                                  *p_noise_y,
    std::function<void(Array &, Vec4<float>, Array *, Array *)> unary_op);

[[deprecated]] void transform(
    Heightmap                                             &h,
    std::function<void(Array &, Vec2<float>, Vec2<float>)> unary_op);

// input array and mask
[[deprecated]] void transform(Heightmap                            &h,
                              Heightmap                            *p_mask,
                              std::function<void(Array &, Array *)> unary_op);

// for erosion
[[deprecated]] void transform(
    Heightmap       &h,
    hmap::Heightmap *p_1,
    hmap::Heightmap *p_2,
    hmap::Heightmap *p_3,
    hmap::Heightmap *p_4,
    hmap::Heightmap *p_5,
    std::function<void(Array &, Array *, Array *, Array *, Array *, Array *)>
        unary_op);

[[deprecated]] void transform(
    Heightmap                                              &h,
    hmap::Heightmap                                        *p_1,
    hmap::Heightmap                                        *p_2,
    hmap::Heightmap                                        *p_3,
    std::function<void(Array &, Array *, Array *, Array *)> unary_op);

[[deprecated]] void transform(
    Heightmap                                     &h,
    hmap::Heightmap                               *p_1,
    hmap::Heightmap                               *p_2,
    std::function<void(Array &, Array *, Array *)> unary_op);

[[deprecated]] void transform(Heightmap                            &h1,
                              Heightmap                            &h2,
                              std::function<void(Array &, Array &)> binary_op);

[[deprecated]] void transform(
    Heightmap                                         &h1,
    Heightmap                                         &h2,
    std::function<void(Array &, Array &, Vec4<float>)> binary_op);

[[deprecated]] void transform(
    Heightmap                                     &h1,
    Heightmap                                     &h2,
    Heightmap                                     &h3,
    std::function<void(Array &, Array &, Array &)> ternary_op);

[[deprecated]] void transform(
    Heightmap                                                  &h1,
    Heightmap                                                  &h2,
    Heightmap                                                  &h3,
    std::function<void(Array &, Array &, Array &, Vec4<float>)> ternary_op);

// with returned array
[[deprecated]] void transform(Heightmap                    &h_out, // output
                              Heightmap                    &h1,    // in 1
                              std::function<Array(Array &)> unary_op);

[[deprecated]] void transform(Heightmap &h_out, // output
                              Heightmap &h1,    // in 1
                              Heightmap &h2,    // in 2
                              std::function<Array(Array &, Array &)> binary_op);

[[deprecated]] void transform(
    Heightmap                                              &h1,
    Heightmap                                              &h2,
    Heightmap                                              &h3,
    Heightmap                                              &h4,
    std::function<void(Array &, Array &, Array &, Array &)> ternary_op);

[[deprecated]] void transform(
    Heightmap &h1,
    Heightmap &h2,
    Heightmap &h3,
    Heightmap &h4,
    Heightmap &h5,
    Heightmap &h6,
    std::function<void(Array &, Array &, Array &, Array &, Array &, Array &)>
        op);

/**
 * @brief Applies a transformation operation to a collection of heightmaps.
 *
 * @param p_hmaps        A vector of pointers to Heightmap objects to be
 *                       transformed.
 * @param op             A function that defines the transformation operation.
 *                       It takes a vector of Array pointers, a Vec2<int>
 *                       representing dimensions, and a Vec4<float> representing
 * transformation parameters.
 * @param transform_mode The mode of transformation to be applied. Default is
 *                       TransformMode::DISTRIBUTED.
 */
void transform(std::vector<Heightmap *>                     p_hmaps,
               std::function<void(const std::vector<Array *>,
                                  const hmap::Vec2<int>,
                                  const hmap::Vec4<float>)> op,
               TransformMode transform_mode = TransformMode::DISTRIBUTED);

void transform(std::vector<Heightmap *>                        p_hmaps,
               std::function<void(const std::vector<Array *>)> op,
               TransformMode transform_mode = TransformMode::DISTRIBUTED);

} // namespace hmap
