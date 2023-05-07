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

#include <vector>

/**
 * @brief Array class, helper to manipulate 2D float array with "(i, j)"
 * indexing.
 *
 */
class Array
{
public:
  /**
   * @brief Construct a new Array object.
   *
   * @param shape Array shape {ni, nj}.
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
  inline std::vector<float> get_vector()
  {
    return vector;
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
  Array operator=(const float value);

  /**
   * @brief Multiplication overloading (right multiply by a scalar).
   *
   * @param value Scalar value.
   * @return Array Reference to the current object.
   */
  Array operator*(const float value); ///< @overload

  /**
   * @brief Multiplication overloading (element-wise product by another array).
   *
   * @param array Another Array.
   * @return Array
   */
  Array operator*(const Array &array); ///< @overload

  /**
   * @brief Multiplication overloading (left multiply by a scalar).
   *
   * @param value Scalar value.
   * @param array Another Array.
   * @return Array Reference to the resulting object.
   */
  friend Array operator*(const float value, const Array &array); ///< @overload

  Array        operator/(const float value);                     ///< @overload
  Array        operator/(const Array &array);                    ///< @overload
  friend Array operator/(const float value, const Array &array); ///< @overload

  Array        operator+(const float value);                     ///< @overload
  Array        operator+(const Array &array);                    ///< @overload
  friend Array operator+(const float value, const Array &array); ///< @overload

  Array        operator-();
  Array        operator-(const float value);                     ///< @overload
  Array        operator-(const Array &array);                    ///< @overload
  friend Array operator-(const float value, const Array &array); ///< @overload

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

private:
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
};
