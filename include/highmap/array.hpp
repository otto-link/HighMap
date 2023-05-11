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

#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

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
  std::vector<int> shape;

  /**
   * @brief Vector for data storage, size shape[0] * shape[1].
   *
   */
  std::vector<float> vector;

  /**
   * @brief Construct a new Array object.
   *
   * @param shape Array shape {ni, nj}.
   *
   * **Example**
   * @include ex_array.cpp
   *
   *
   */
  Array(std::vector<int> shape);

  //----------------------------------------
  // accessors
  //----------------------------------------

  /**
   * @brief Get the shape object.
   *
   * @return std::vector<int> Shape {ni, nj}.
   */
  inline std::vector<int> get_shape()
  {
    return shape;
  }

  /**
   * @brief Get the vector object.
   *
   * @return std::vector<float> Vector of size shape[0] * shape[1].
   */
  std::vector<float> get_vector()
  {
    return this->vector;
  }

  //----------------------------------------
  // overload
  //----------------------------------------

  /**
   * @brief Assignment overloading (scalar assignement).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator=(float value);

  /**
   * @brief Multiplication overloading (right multiply by a scalar).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator*(float value);

  /**
   * @brief Multiplication overloading (element-wise product by another array).
   *
   * @param array Another Array.
   * @return Array
   */
  Array operator*(const Array &array);

  /**
   * @brief Multiplication overloading (left multiply by a scalar).
   *
   * @param value Scalar value.
   * @param array Another Array.
   * @return Array Reference to the resulting object.
   */
  friend Array operator*(float value, const Array &array);

  /**
   * @brief Division overloading (right divide by a scalar).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator/(float value);

  /**
   * @brief Division overloading (element-wise division by another array).
   *
   * @param array Another Array.
   * @return Array
   */
  Array operator/(const Array &array);

  /**
   * @brief Division overloading (left divide by a scalar).
   *
   * @param value Scalar value.
   * @param array Another Array.
   * @return Array Reference to the resulting object.
   */
  friend Array operator/(float value, const Array &array);

  /**
   * @brief Addition overloading (right add by a scalar).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator+(float value);

  /**
   * @brief Addition overloading (element-wise addition by another array).
   *
   * @param array Another Array.
   * @return Array
   */
  Array operator+(const Array &array);

  /**
   * @brief Addition overloading (left add by a scalar).
   *
   * @param value Scalar value.
   * @param array Another Array.
   * @return Array Reference to the resulting object.
   */
  friend Array operator+(float value, const Array &array);

  /**
   * @brief Unary minus overloading.
   *
   * @return Array Reference to the current object.
   */
  Array operator-();

  /**
   * @brief Subtraction overloading (right substract by a scalar).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator-(float value);

  /**
   * @brief Subtraction overloading (element-wise substract by another array).
   *
   * @param array Another Array.
   * @return Array
   */
  Array operator-(const Array &array);

  /**
   * @brief Subtraction overloading (left substract by a scalar).
   *
   * @param value Scalar value.
   * @param array Another Array.
   * @return Array Reference to the resulting object.
   */
  friend Array operator-(float value, const Array &array);

  /**
   * @brief Call overloading, return array value at index (i, j).
   *
   * @param i 'i' index.
   * @param j 'j' index.
   * @return float& Array value at index (i, j).
   */
  inline float &operator()(int i, int j)
  {
    return this->vector[i * this->shape[1] + j];
  }

  //----------------------------------------
  // methods
  //----------------------------------------

  /**
   * @brief Return the value of the greastest element in the array.
   *
   * @return float
   */
  inline float max()
  {
    return *std::max_element(this->vector.begin(), this->vector.end());
  }

  /**
   * @brief Return the value of the smallest element in the array.
   *
   * @return float
   */
  inline float min()
  {
    return *std::min_element(this->vector.begin(), this->vector.end());
  }

  /**
   * @brief Normalize array values so that the array sum is equal to 1.
   *
   */
  inline void normalize()
  {
    float sum = this->sum();

    std::transform(this->vector.begin(),
                   this->vector.end(),
                   this->vector.begin(),
                   [&sum](float v) { return v / sum; });
  }

  /**
   * @brief Return the peak-to-peak amplitude (i.e. max - min) of the array
   * values.
   *
   * @return float
   */
  inline float ptp()
  {
    return this->max() - this->min();
  }

  /**
   * @brief Return of the array values.
   *
   * @return float
   */
  inline float sum()
  {
    return std::accumulate(this->vector.begin(), this->vector.end(), 0.f);
  }

  /**
   * @brief Export array as png image file.
   *
   * @param fname File name.
   * @param cmap Colormap (@see cmap).
   *
   * **Example**
   * @include ex_perlin.cpp
   */
  void to_png(std::string fname, int cmap);
};

} // namespace hmap
