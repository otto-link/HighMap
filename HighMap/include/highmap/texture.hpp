/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file texture.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for the Texture class.
 *
 * This file contains the definition of the `Texture` class, which is used for
 * representing and manipulating multi-channel 2D arrays of floating-point
 * values (textures) using planar hmap::Array storage.
 */

#pragma once
#include <string>
#include <vector>

#include <opencv2/core.hpp>

#include "highmap/algebra.hpp"
#include "highmap/array.hpp"

namespace hmap
{

/**
 * @class Texture
 * @brief A class to represent a multi-channel texture using planar hmap::Array
 * storage.
 *
 * The `Texture` class represents a multi-channel image/texture. Each channel is
 * stored as a separate `Array` object, allowing direct use of `Array` functions
 * and operators on individual channels without copying.
 */
class Texture
{
public:
  /**
   * @brief Shape of the texture in 2D space (x, y).
   */
  glm::ivec2 shape = glm::ivec2(0, 0);

  /**
   * @brief Vector of planar Array channels.
   */
  std::vector<Array> channels;

  /**
   * @brief Construct an empty Texture object.
   */
  Texture() = default;

  /**
   * @brief Construct a new Texture object with shape and channel count.
   *
   * @param shape        2D shape of the texture.
   * @param num_channels Number of channels.
   */
  Texture(glm::ivec2 shape, int num_channels);

  /**
   * @brief Construct a new Texture object with shape, channel count and fill
   * value.
   *
   * @param shape        2D shape of the texture.
   * @param num_channels Number of channels.
   * @param fill_value   The value to fill all channels with.
   */
  Texture(glm::ivec2 shape, int num_channels, float fill_value);

  /**
   * @brief Construct a single-channel Texture from an Array.
   *
   * @param array The single channel Array.
   */
  explicit Texture(const Array &array);

  /**
   * @brief Construct a 3-channel Texture from three Arrays.
   */
  Texture(const Array &r, const Array &g, const Array &b);

  /**
   * @brief Construct a 4-channel Texture from four Arrays.
   */
  Texture(const Array &r, const Array &g, const Array &b, const Array &a);

  /**
   * @brief Construct a Texture from a list of Arrays.
   */
  explicit Texture(const std::vector<Array> &channels);

  /**
   * @brief Constructs a new Texture object from an image file.
   *
   * @param fname  The name of the file to load the texture from.
   * @param flip_j If true, flip the image vertically on load.
   */
  explicit Texture(const std::string &fname, bool flip_j = false);

  /**
   * @brief Check whether the texture is empty (no channels or zero area).
   */
  bool is_empty() const
  {
    return channels.empty() || shape.x <= 0 || shape.y <= 0;
  }

  /**
   * @brief Get the number of channels.
   */
  int num_channels() const
  {
    return static_cast<int>(channels.size());
  }

  /**
   * @brief Access channel Array by index.
   */
  Array &operator[](size_t idx)
  {
    return channels[idx];
  }
  const Array &operator[](size_t idx) const
  {
    return channels[idx];
  }

  /**
   * @brief Access channel Array by index.
   */
  Array &channel(size_t idx)
  {
    return channels[idx];
  }
  const Array &channel(size_t idx) const
  {
    return channels[idx];
  }

  /**
   * @brief Access element at (i, j) in channel c.
   */
  float &operator()(int i, int j, int c)
  {
    return channels[c](i, j);
  }

  /**
   * @brief Access element at (i, j) in channel c (const).
   */
  const float &operator()(int i, int j, int c) const
  {
    return channels[c](i, j);
  }

  /**
   * @brief Read 3-channel color at (i, j) as glm::vec3.
   */
  glm::vec3 get_pixel3(int i, int j) const;

  /**
   * @brief Read 4-channel color at (i, j) as glm::vec4.
   */
  glm::vec4 get_pixel4(int i, int j) const;

  /**
   * @brief Write 3-channel color at (i, j) from glm::vec3.
   */
  void set_pixel(int i, int j, const glm::vec3 &color);

  /**
   * @brief Write 4-channel color at (i, j) from glm::vec4.
   */
  void set_pixel(int i, int j, const glm::vec4 &color);

  /**
   * @brief Find the maximum value in all channels.
   */
  float max() const;

  /**
   * @brief Find the minimum value in all channels.
   */
  float min() const;

  /**
   * @brief Remap the texture values to a new range.
   */
  void remap(float vmin = 0.f, float vmax = 1.f);

  /**
   * @brief Resamples the texture to a new 2D shape (x, y).
   */
  Texture resample_to_shape(glm::ivec2 new_shape_xy) const;

  /**
   * @brief Convert the texture to an OpenCV matrix.
   */
  cv::Mat to_cv_mat() const;

  /**
   * @brief Convert the texture to an 8-bit image represented as a vector.
   */
  std::vector<uint8_t> to_img_8bit(bool flip_y = false) const;

  /**
   * @brief Saves the Texture as an image file (e.g. PNG).
   */
  void to_png(const std::string &fname, int depth = CV_8U) const;
};

} // namespace hmap
