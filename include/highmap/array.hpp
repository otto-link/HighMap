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
   * @brief Distribute a value 'amount' around the cell (i, j) using a
   * deposition kernel.
   *
   * @param i Index.
   * @param j Index.
   * @param kernel Deposition kernel, must have an odd number of elements in
   * each direction.
   * @param amount Amount to be deposited.
   */
  inline void depose_amount_kernel(int i, int j, Array &kernel, float amount)
  {
    const int ir = (kernel.shape[0] - 1) / 2;
    const int jr = (kernel.shape[1] - 1) / 2;

    for (int p = 0; p < kernel.shape[0]; p++)
    {
      for (int q = 0; q < kernel.shape[1]; q++)
      {
        (*this)(i + p - ir, j + q - jr) += amount * kernel(p, q);
      }
    }
  }

  /**
   * @brief Return the gradient in the 'x' (or 'i' index) of at the index (i,
   * j).
   *
   * @warning Based on a 2nd order central difference scheme, cannot be used
   * at the borders, i.e. for i = 0, j = 0, i = shape[0] - 1 or j =
   * shape[1] - 1.
   *
   * @param i Index, expected to be in [1, shape[0] - 2].
   * @param j Index, expected to be in [1, shape[1] - 2].
   * @return float
   */
  inline float get_gradient_x_at(int i, int j)
  {
    return 0.5f * ((*this)(i + 1, j) - (*this)(i - 1, j));
  }

  /**
   * @brief Return the gradient in the 'y' (or 'j' index) of at the index (i,
   * j).
   *
   * @warning Based on a 2nd order central difference scheme, cannot be used
   * at the borders, i.e. for i = 0, j = 0, i = shape[0] - 1 or j =
   * shape[1] - 1.
   *
   * @param i Index, expected to be in [1, shape[0] - 2].
   * @param j Index, expected to be in [1, shape[1] - 2].
   * @return float
   */
  inline float get_gradient_y_at(int i, int j)
  {
    return 0.5f * ((*this)(i, j + 1) - (*this)(i, j - 1));
  }

  /**
   * @brief Return the gradient in the 'x' (or 'i' index) of at the location (x,
   * y) near the index (i, j) using bilinear interpolation.
   *
   * @warning Based on a 2nd order central difference scheme, cannot be used
   * at the borders, i.e. for i = 0, j = 0, i = shape[0] - 1 or j =
   * shape[1] - 1.
   *
   * @param i Index, expected to be in [1, shape[0] - 2].
   * @param j Index, expected to be in [1, shape[1] - 2].
   * @param x 'x' coordinate, expected to be in [i, i + 1].
   * @param y 'y' coordinate, expected to be in [j, j + 1].
   * @return float
   */
  inline float get_gradient_x_bilinear_at(int i, int j, float u, float v)
  {
    float f00 = (*this)(i, j) - (*this)(i - 1, j);
    float f10 = (*this)(i + 1, j) - (*this)(i, j);
    float f01 = (*this)(i, j + 1) - (*this)(i - 1, j + 1);
    float f11 = (*this)(i + 1, j + 1) - (*this)(i, j + 1);

    float a10 = f10 - f00;
    float a01 = f01 - f00;
    float a11 = f11 - f10 - f01 + f00;

    return f00 + a10 * u + a01 * v + a11 * u * v;
  }

  /**
   * @brief Return the gradient in the 'y' (or 'j' index) of at the location (x,
   * y) near the index (i, j) using bilinear interpolation.
   *
   * @warning Based on a 2nd order central difference scheme, cannot be used at
   * the borders, i.e. for i = 0, j = 0, i = shape[0] - 1 or j = shape[1] - 1.
   *
   * @param i Index, expected to be in [1, shape[0] - 2].
   * @param j Index, expected to be in [1, shape[1] - 2].
   * @param x 'x' coordinate, expected to be in [i, i + 1].
   * @param y 'y' coordinate, expected to be in [j, j + 1].
   * @return float
   */
  inline float get_gradient_y_bilinear_at(int i, int j, float u, float v)
  {
    float f00 = (*this)(i, j) - (*this)(i, j - 1);
    float f10 = (*this)(i + 1, j) - (*this)(i + 1, j - 1);
    float f01 = (*this)(i, j + 1) - (*this)(i, j);
    float f11 = (*this)(i + 1, j + 1) - (*this)(i + 1, j);

    float a10 = f10 - f00;
    float a01 = f01 - f00;
    float a11 = f11 - f10 - f01 + f00;

    return f00 + a10 * u + a01 * v + a11 * u * v;
  }

  /**
   * @brief Return the array value at the location (x, y) near the index (i, j)
   * using bilinear interpolation.
   *
   * @warning Based on bilinear interpolation, cannot be used at the upper
   * borders, i.e. for i = shape[0] - 1 or j = shape[1] - 1.
   *
   * @param i Index, expected to be in [0, shape[0] - 2].
   * @param j Index, expected to be in [0, shape[1] - 2].
   * @param x 'x' coordinate, expected to be in [i, i + 1].
   * @param y 'y' coordinate, expected to be in [j, j + 1].
   * @return float
   */
  inline float get_value_bilinear_at(int i, int j, float u, float v)
  {
    float a10 = (*this)(i + 1, j) - (*this)(i, j);
    float a01 = (*this)(i, j + 1) - (*this)(i, j);
    float a11 = (*this)(i + 1, j + 1) - (*this)(i + 1, j) - (*this)(i, j + 1) +
                (*this)(i, j);

    return (*this)(i, j) + a10 * u + a01 * v + a11 * u * v;
  }

  /**
   * @brief Return stacked arrays in sequence horizontally (column wise).
   *
   * @param array1 1st array.
   * @param array2 2st array.
   * @return Array Reference to the resulting object.
   */
  friend Array hstack(Array &array1, Array &array2);

  /**
   * @brief Display a bunch of infos on the array.
   *
   */
  void infos(std::string msg = "");

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
  Array resample_to_shape(std::vector<int> new_shape);

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
   *
   * **Example**
   * @include ex_perlin.cpp
   */
  void to_png(std::string fname, int cmap);

  /**
   * @brief Return stacked arrays in sequence vertically (row wise).
   *
   * @param array1 1st array.
   * @param array2 2st array.
   * @return Array Reference to the resulting object.
   */
  friend Array vstack(Array &array1, Array &array2);
};

} // namespace hmap
