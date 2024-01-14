/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

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
typedef unsigned int uint;

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <png.h>

#include "highmap/vector.hpp"

namespace hmap
{

/**
 * @brief Array class, helper to manipulate 2D float array with "(i, j)"
 * indexing.
 *
 */
class Array
{
public:
  /**
   * @brief Array shape {ni, nj}.
   *
   */
  Vec2<int> shape;

  /**
   * @brief Vector for data storage, size shape.x * shape.y.
   *
   */
  std::vector<float> vector;

  /**
   * @brief Construct a new Array object.
   *
   * @param shape Array shape {ni, nj}.
   * @param value Array filling value at creation.
   *
   * **Example #1**
   * @include ex_array.cpp

   * **Example #2**
   * @include ex_to_png.cpp
   *
   */
  Array();

  Array(Vec2<int> shape); ///< @overload

  Array(Vec2<int> shape, float value); ///< @overload

  Array(std::string filename, bool resize_array = true); ///< @overload

  //----------------------------------------
  // accessors
  //----------------------------------------

  /**
   * @brief Get the shape object.
   *
   * @return Vec2<int> Shape {ni, nj}.
   */
  Vec2<int> get_shape();

  /**
   * @brief Get the vector object.
   *
   * @return std::vector<float> Vector of size shape.x * shape.y.
   */
  std::vector<float> get_vector() const;

  /**
   * @brief Set the array shape.
   *
   * @param new_shape New shape.
   */
  void set_shape(Vec2<int> new_shape);

  //----------------------------------------
  // overload
  //----------------------------------------

  /**
   * @brief Assignment overloading (scalar assignement).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator=(const float value);

  /**
   * @brief Division-assignment overloading (scalar).
   *
   * @param value Scalar value.
   */
  Array operator*=(const float value);

  Array operator*=(const Array &array); ///< @overload

  /**
   * @brief Division-assignment overloading (scalar).
   *
   * @param value Scalar value.
   */
  Array operator/=(const float value);

  Array operator/=(const Array &array); ///< @overload

  /**
   * @brief Division-assignment overloading (scalar).
   *
   * @param value Scalar value.
   */
  Array operator+=(const float value);

  Array operator+=(const Array &array); ///< @overload

  /**
   * @brief Division-assignment overloading (scalar).
   *
   * @param value Scalar value.
   */
  Array operator-=(const float value);

  Array operator-=(const Array &array); ///< @overload

  /**
   * @brief Multiplication overloading (right multiply by a scalar).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator*(const float value) const;

  /**
   * @brief Multiplication overloading (element-wise product by another array).
   *
   * @param array Another Array.
   * @return Array Reference to the resulting object.
   */
  Array operator*(const Array &array) const;

  /**
   * @brief Multiplication overloading (left multiply by a scalar).
   *
   * @param value Scalar value.
   * @param array Another Array.
   * @return Array Reference to the resulting object.
   */
  friend Array operator*(const float value, const Array &array);

  /**
   * @brief Division overloading (right divide by a scalar).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator/(const float value) const;

  /**
   * @brief Division overloading (element-wise division by another array).
   *
   * @param array Another Array.
   * @return Array
   */
  Array operator/(const Array &array) const;

  /**
   * @brief Division overloading (left divide by a scalar).
   *
   * @param value Scalar value.
   * @param array Another Array.
   * @return Array Reference to the resulting object.
   */
  friend Array operator/(const float value, const Array &array);

  /**
   * @brief Addition overloading (right add by a scalar).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator+(const float value) const;

  /**
   * @brief Addition overloading (element-wise addition by another array).
   *
   * @param array Another Array.
   * @return Array
   */
  Array operator+(const Array &array) const;

  /**
   * @brief Addition overloading (left add by a scalar).
   *
   * @param value Scalar value.
   * @param array Another Array.
   * @return Array Reference to the resulting object.
   */
  friend Array operator+(const float value, const Array &array);

  /**
   * @brief Unary minus overloading.
   *
   * @return Array Reference to the current object.
   */
  Array operator-() const;

  /**
   * @brief Subtraction overloading (right substract by a scalar).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator-(const float value) const;

  /**
   * @brief Subtraction overloading (element-wise substract by another array).
   *
   * @param array Another Array.
   * @return Array
   */
  Array operator-(const Array &array) const;

  /**
   * @brief Subtraction overloading (left substract by a scalar).
   *
   * @param value Scalar value.
   * @param array Another Array.
   * @return Array Reference to the resulting object.
   */
  friend const Array operator-(const float value, const Array &array);

  /**
   * @brief Call overloading, return array value at index (i, j).
   *
   * @param i 'i' index.
   * @param j 'j' index.
   * @return float& Array value at index (i, j).
   */

  float &operator()(int i, int j)
  {
    return this->vector[i * this->shape.y + j];
  }

  const float &operator()(int i, int j) const ///< @overload
  {
    return this->vector[i * this->shape.y + j];
  }

  //----------------------------------------
  // methods
  //----------------------------------------

  /**
   * @brief Add to a given array a kernel (with a smaller shape), centered on
   * indices (i, j).
   *
   * @param array Input Array.
   * @param kernel Kernel to add.
   * @param i Index i (for the input array) where the kernel is added.
   * @param j Index j (for the input array) where the kernel is added.
   */
  friend void add_kernel(Array &array, const Array &kernel, int i, int j);

  /**
   * @brief Return a column 'j' as a std::vector.
   *
   * @param j Colunm index.
   * @return std::vector<float>
   */
  std::vector<float> col_to_vector(int j);

  /**
   * @brief Distribute a value 'amount' around the four cells (i, j), (i + 1,
   * j), (i, j + 1), (i + 1, j + 1) by "reversing" the bilinear interpolation.
   *
   * @param i Index.
   * @param j Index.
   * @param u 'u' interpolation parameter, expected to be in [0, 1[.
   * @param v 'v' interpolation parameter, expected to be in [0, 1[.
   * @param amount Amount to be deposited.
   */
  void depose_amount_bilinear_at(int i, int j, float u, float v, float amount);

  void depose_amount_kernel_bilinear_at(int   i,
                                        int   j,
                                        float u,
                                        float v,
                                        int   ir,
                                        float amount);

  /**
   * @brief Distribute a value 'amount' around the cell (i, j) using a
   * a 1D deposition kernel (applied to both direction).
   *
   * @param i Index.
   * @param j Index.
   * @param kernel Deposition kernel (1D), must have an odd number of elements.
   * @param amount Amount to be deposited.
   */
  void depose_amount_kernel_at(int i, int j, Array &kernel, float amount);

  /**
   * @brief Extract the value of a slice {i1, i2, j1, j2} to create a new array.
   *
   * @param idx Slice extent indices: {i1, i2, j1, j2}.
   * @return Array Resulting array.
   */
  Array extract_slice(Vec4<int> idx);

  /**
   * @brief Find the lowest elevation and elevation difference path between two
   * points in a 2d-array.
   *
   * @see @cite Dijkstra1971 and
   * https://math.stackexchange.com/questions/3088292.
   *
   * @warning Parameter `elevation_ratio` must be < 1 to permit convergence.
   *
   * @param ij_start Starting point (i, j) position.
   * @param ij_end Ending point (i, j) position.
   * @param i_path[out] Shortest path 'i' indices.
   * @param j_path[out] Shortest path 'j' indices.
   * @param elevation_ratio Balance between absolute elevation and elevation
   * difference in the cost function, in [0, 1[.
   * @param distance_exponent Exponent of the distance calculation between two
   * points. Increasing the "distance exponent" of the cost function increases
   * the cost of elevation gaps: path then tends to stay at the same elevation
   * if possible (i.e. reduce the overall cumulative elevation gain).
   * @param step Steps for i and j indices, to accelerate computation by using
   * only every "step" data.
   * @param p_mask_nogo Reference to the mask defining areas to avoid.
   *
   * **Example**
   * @include ex_find_path_dijkstra.cpp
   *
   * **Result**
   * @image html ex_find_path_dijkstra0.png
   * @image html ex_find_path_dijkstra1.png
   * @image html ex_find_path_dijkstra2.png
   */
  void find_path_dijkstra(Vec2<int>         ij_start,
                          Vec2<int>         ij_end,
                          std::vector<int> &i_path,
                          std::vector<int> &j_path,
                          float             elevation_ratio = 0.1f,
                          float             distance_exponent = 2.f,
                          Vec2<int>         step = {1, 1},
                          Array            *p_mask_nogo = nullptr);

  /**
   * @brief Return the gradient in the 'x' (or 'i' index) of at the index (i,
   * j).
   *
   * @warning Based on a 2nd order central difference scheme, cannot be used
   * at the borders, i.e. for i = 0, j = 0, i = shape.x - 1 or j =
   * shape.y - 1.
   *
   * @param i Index, expected to be in [1, shape.x - 2].
   * @param j Index, expected to be in [1, shape.y - 2].
   * @return float
   */
  float get_gradient_x_at(int i, int j) const;

  /**
   * @brief Return the gradient in the 'y' (or 'j' index) of at the index (i,
   * j).
   *
   * @warning Based on a 2nd order central difference scheme, cannot be used
   * at the borders, i.e. for i = 0, j = 0, i = shape.x - 1 or j =
   * shape.y - 1.
   *
   * @param i Index, expected to be in [1, shape.x - 2].
   * @param j Index, expected to be in [1, shape.y - 2].
   * @return float
   */
  float get_gradient_y_at(int i, int j) const;

  /**
   * @brief Return the gradient in the 'x' (or 'i' index) of at the location (x,
   * y) near the index (i, j) using bilinear interpolation.
   *
   * @warning Based on a 2nd order central difference scheme, cannot be used
   * at the borders, i.e. for i = 0, j = 0, i = shape.x - 1 or j =
   * shape.y - 1.
   *
   * @param i Index, expected to be in [1, shape.x - 2].
   * @param j Index, expected to be in [1, shape.y - 2].
   * @param u 'u' interpolation parameter, expected to be in [0, 1[.
   * @param v 'v' interpolation parameter, expected to be in [0, 1[.
   * @return float
   */
  float get_gradient_x_bilinear_at(int i, int j, float u, float v) const;

  /**
   * @brief Return the gradient in the 'y' (or 'j' index) of at the location (x,
   * y) near the index (i, j) using bilinear interpolation.
   *
   * @warning Based on a 2nd order central difference scheme, cannot be used at
   * the borders, i.e. for i = 0, j = 0, i = shape.x - 1 or j = shape.y - 1.
   *
   * @param i Index, expected to be in [1, shape.x - 2].
   * @param j Index, expected to be in [1, shape.y - 2].
   * @param u 'u' interpolation parameter, expected to be in [0, 1[.
   * @param v 'v' interpolation parameter, expected to be in [0, 1[.
   * @return float
   */
  float get_gradient_y_bilinear_at(int i, int j, float u, float v) const;

  /**
   * @brief Return the surface normal at the index (i, j).
   *
   * @param i Index.
   * @param j Index.
   * @return Vec3<float> Normal vector (3 components).
   */
  Vec3<float> get_normal_at(int i, int j) const;

  /**
   * @brief Return normalization coefficients (a, b) so that a * array + b is in
   * [vmin, vmax]
   *
   * @param vmin Lower bound.
   * @param vmax Upper bound.
   * @return Vec2<float> Coefficients (a, b)
   */
  Vec2<float> normalization_coeff(float vmin = 0.f, float vmax = 1.f);

  /**
   * @brief Return the array value at the location (x, y) near the index (i, j)
   * using bilinear interpolation.
   *
   * @warning Based on bilinear interpolation, cannot be used at the upper
   * borders, i.e. for i = shape.x - 1 or j = shape.y - 1.
   *
   * @param i Index, expected to be in [0, shape.x - 2].
   * @param j Index, expected to be in [0, shape.y - 2].
   * @param u 'u' interpolation parameter, expected to be in [0, 1[.
   * @param v 'v' interpolation parameter, expected to be in [0, 1[.
   * @return float
   */
  float get_value_bilinear_at(int i, int j, float u, float v) const;

  /**
   * @brief Return stacked arrays in sequence horizontally (column wise).
   *
   * @param array1 1st array.
   * @param array2 2st array.
   * @return Array Reference to the resulting object.
   */
  friend Array hstack(const Array &array1, const Array &array2);

  /**
   * @brief Import array from raw binary file.
   *
   * @param fname File name.
   */
  void from_file(std::string fname);

  /**
   * @brief Display a bunch of infos on the array.
   *
   */
  void infos(std::string msg = "") const;

  /**
   * @brief Return the linear index corresponding to the (i, j) cell.
   *
   * @param i 'i' index.
   * @param j 'j' index.
   * @return int Linear index.
   */
  int linear_index(int i, int j) const;

  /**
   * @brief Return the value of the greastest element in the array.
   *
   * @return float
   */
  float max() const;

  /**
   * @brief Return the mean value of the element in the array.
   *
   * @return float
   */
  float mean() const;

  /**
   * @brief Return the value of the smallest element in the array.
   *
   * @return float
   */
  float min() const;

  /**
   * @brief Normalize array values so that the array sum is equal to 1.
   *
   */
  void normalize();

  /**
   * @brief Print vector values to stdout.
   *
   */
  void print();

  /**
   * @brief Return the peak-to-peak amplitude (i.e. max - min) of the array
   * values.
   *
   * @return float
   */
  float ptp() const;

  /**
   * @brief Return a resampled array of shape 'new_shape' using bilinear
   * interpolation.
   *
   * @param new_shape Target shape.
   * @return Array Resampled array.
   *
   * **Example**
   * @include ex_resample_to_shape.cpp
   *
   * **Result**
   * @image html ex_resample_to_shape.png
   */
  Array resample_to_shape(Vec2<int> new_shape);

  /**
   * @brief Return a column 'i' as a std::vector.
   *
   * @param i Row index.
   * @return std::vector<float>
   */
  std::vector<float> row_to_vector(int i);

  /**
   * @brief Set the value of a slice {i1, i2, j1, j2} of data.
   *
   * @param idx Slice extent indices: {i1, i2, j1, j2}.
   * @param value New value.
   */
  void set_slice(Vec4<int> idx, float value);

  /**
   * @brief Return the array size (number of elements).
   *
   * @return int
   */
  int size() const;

  /**
   * @brief Return of the array values.
   *
   * @return float
   */
  float sum() const;

  /**
   * @brief Export array a raw binary file.
   *
   * @param fname File name.
   */
  void to_file(std::string fname);

  /**
   * @brief Export array as png image file.
   *
   * @param fname File name.
   * @param cmap Colormap (@see cmap).
   * @param hillshading Activate hillshading.
   *
   * **Example**
   * @include ex_to_png.cpp
   */
  void to_png(std::string fname, int cmap, bool hillshading = false);

  /**
   * @brief Export array as 8bit grayscale png image file.
   *
   * @param fname File name.
   *
   * **Example**
   * @include ex_to_png.cpp
   */
  void to_png_grayscale_8bit(std::string fname);

  /**
   * @brief Export array as 16bit grayscale png image file.
   *
   * @param fname File name.
   *
   * **Example**
   * @include ex_to_png.cpp
   */
  void to_png_grayscale_16bit(std::string fname);

  /**
   * @brief Export array as 16bit raw file (Unity terrain import format).
   *
   * @param fname File name.
   *
   * **Example**
   * @include ex_to_png.cpp
   */
  void to_raw_16bit(std::string fname);

  /**
   * @brief Returns the unique elements of the array.
   *
   * @return std::vector<float> Unique values.
   */
  std::vector<float> unique_values();

  /**
   * @brief Return stacked arrays in sequence vertically (row wise).
   *
   * @param array1 1st array.
   * @param array2 2st array.
   * @return Array Reference to the resulting object.
   */
  friend Array vstack(const Array &array1, const Array &array2);
};

} // namespace hmap
