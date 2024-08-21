/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file tensor.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Header file for the Tensor class.
 *
 * This file contains the definition of the `Tensor` class, which is used for
 * representing and manipulating multi-dimensional arrays of floating-point
 * values. The class provides methods for accessing elements, performing
 * basic mathematical operations, and exporting data to image formats.
 *
 * @version 0.1
 * @date 2023-05-07
 *
 * @copyright Copyright (c) 2023 Otto Link
 *
 */

#pragma once
#include <opencv2/core/mat.hpp>

#include "highmap/algebra.hpp"
#include "highmap/array.hpp"

namespace hmap
{

/**
 * @class Tensor
 * @brief A class to represent a multi-dimensional tensor.
 *
 * The `Tensor` class is used to store and manipulate multi-dimensional arrays
 * (tensors) of floating-point numbers. It supports element access, basic
 * operations such as finding the minimum and maximum values, remapping the
 * tensor values, and exporting the data to images.
 */
class Tensor
{
public:
  /**
   * @brief Shape of the tensor in 3D space.
   */
  Vec3<int> shape;

  /**
   * @brief Flattened vector containing the tensor's elements.
   */
  std::vector<float> vector;

  /**
   * @brief Construct a new Tensor object.
   *
   * @param shape 3D shape of the tensor.
   */
  Tensor(Vec3<int> shape);

  /**
   * @brief Construct a new Tensor object.
   *
   * @param shape_xy 2D shape (x, y) of the tensor.
   * @param shape_z Size along the z-axis.
   */
  Tensor(Vec2<int> shape_xy, int shape_z);

  /**
   * @brief Access an element of the tensor.
   *
   * @param i Index along the x-axis.
   * @param j Index along the y-axis.
   * @param k Index along the z-axis.
   * @return float& Reference to the element at position (i, j, k).
   */
  float &operator()(int i, int j, int k);

  /**
   * @brief Access an element of the tensor (const version).
   *
   * @param i Index along the x-axis.
   * @param j Index along the y-axis.
   * @param k Index along the z-axis.
   * @return const float& Reference to the element at position (i, j, k).
   */
  const float &operator()(int i, int j, int k) const;

  /**
   * @brief Find the maximum value in the tensor.
   *
   * @return float Maximum value in the tensor.
   */
  float max() const;

  /**
   * @brief Find the minimum value in the tensor.
   *
   * @return float Minimum value in the tensor.
   */
  float min() const;

  /**
   * @brief Remap the tensor values to a new range.
   *
   * @param vmin Minimum value of the new range (default is 0.0).
   * @param vmax Maximum value of the new range (default is 1.0).
   */
  void remap(float vmin = 0.f, float vmax = 1.f);

  /**
   * @brief Set a 2D slice of the tensor along the z-axis.
   *
   * @param k Index along the z-axis.
   * @param slice The 2D array (slice) to set.
   */
  void set_slice(int k, const Array &slice);

  /**
   * @brief Convert the tensor to an OpenCV matrix.
   *
   * @return cv::Mat OpenCV matrix representing the tensor.
   */
  cv::Mat to_cv_mat();

  /**
   * @brief Convert the tensor to an 8-bit image represented as a vector.
   *
   * @return std::vector<uint8_t> Vector containing the 8-bit image data.
   */
  std::vector<uint8_t> to_img_8bit();

  /**
   * @brief Export the tensor to an 8-bit PNG image.
   *
   * @param fname File name of the PNG image.
   */
  void to_png_8bit(std::string fname);

  /**
   * @brief Export the tensor to a 16-bit PNG image.
   *
   * @param fname File name of the PNG image.
   */
  void to_png_16bit(std::string fname);
};

} // namespace hmap
