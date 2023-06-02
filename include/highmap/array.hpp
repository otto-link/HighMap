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
#include <cmath>
#include <iomanip>
#include <iostream>
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
  Array operator=(const float value);

  /**
   * @brief Division-assignment overloading (scalar).
   *
   * @param value Scalar value.
   */
  Array operator*=(const float value);

  /**
   * @brief Division-assignment overloading (scalar).
   *
   * @param value Scalar value.
   */
  Array operator/=(const float value);

  /**
   * @brief Division-assignment overloading (scalar).
   *
   * @param value Scalar value.
   */
  Array operator+=(const float value);

  /**
   * @brief Division-assignment overloading (scalar).
   *
   * @param value Scalar value.
   */
  Array operator-=(const float value);

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
  inline float &operator()(int i, int j)
  {
    return this->vector[i * this->shape[1] + j];
  }

  inline const float &operator()(int i, int j) const ///< @overload
  {
    return this->vector[i * this->shape[1] + j];
  }

  //----------------------------------------
  // methods
  //----------------------------------------

  /**
   * @brief Return a column 'j' as a std::vector.
   *
   * @param j Colunm index.
   * @return std::vector<float>
   */
  inline std::vector<float> col_to_vector(int j)
  {
    std::vector<float> vec(this->shape[0]);
    for (int i = 0; i < this->shape[0]; i++)
      vec[i] = (*this)(i, j);
    return vec;
  }

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
  inline void depose_amount_bilinear_at(int   i,
                                        int   j,
                                        float u,
                                        float v,
                                        float amount)
  {
    (*this)(i, j) += amount * (1 - u) * (1 - v);
    (*this)(i + 1, j) += amount * u * (1 - v);
    (*this)(i, j + 1) += amount * (1 - u) * v;
    (*this)(i + 1, j + 1) += amount * u * v;
  }

  inline void depose_amount_kernel_bilinear_at(int   i,
                                               int   j,
                                               float u,
                                               float v,
                                               int   ir,
                                               float amount)
  {
    Array kernel = Array({2 * ir + 1, 2 * ir + 1});

    // compute kernel first
    for (int p = -ir; p < ir + 1; p++)
    {
      for (int q = -ir; q < ir + 1; q++)
      {
        float x = (float)p - u;
        float y = (float)q - v;
        float r = std::max(0.f, 1.f - std::hypot(x, y));
        kernel(p + ir, q + ir) = r;
      }
    }
    kernel.normalize();

    // perform deposition
    this->depose_amount_kernel_at(i, j, kernel, amount);
  }

  /**
   * @brief Distribute a value 'amount' around the cell (i, j) using a
   * a 1D deposition kernel (applied to both direction).
   *
   * @param i Index.
   * @param j Index.
   * @param kernel Deposition kernel (1D), must have an odd number of elements.
   * @param amount Amount to be deposited.
   */
  inline void depose_amount_kernel_at(int i, int j, Array &kernel, float amount)
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
   * @param u 'u' interpolation parameter, expected to be in [0, 1[.
   * @param v 'v' interpolation parameter, expected to be in [0, 1[.
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
   * @param u 'u' interpolation parameter, expected to be in [0, 1[.
   * @param v 'v' interpolation parameter, expected to be in [0, 1[.
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
   * @param u 'u' interpolation parameter, expected to be in [0, 1[.
   * @param v 'v' interpolation parameter, expected to be in [0, 1[.
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
  inline float max() const
  {
    return *std::max_element(this->vector.begin(), this->vector.end());
  }

  /**
   * @brief Return the value of the smallest element in the array.
   *
   * @return float
   */
  inline float min() const
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
   * @brief Print vector values to stdout.
   *
   */
  inline void print()
  {
    std::cout << std::fixed << std::setprecision(6) << std::setfill('0');
    for (int j = shape[1] - 1; j > -1; j--)
    {
      for (int i = 0; i < shape[0]; i++)
      {
        std::cout << std::setw(5) << (*this)(i, j) << " ";
      }
      std::cout << std::endl;
    }
  }

  /**
   * @brief Return the peak-to-peak amplitude (i.e. max - min) of the array
   * values.
   *
   * @return float
   */
  inline float ptp() const
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
   * @brief Return a column 'i' as a std::vector.
   *
   * @param i Row index.
   * @return std::vector<float>
   */
  inline std::vector<float> row_to_vector(int i)
  {
    std::vector<float> vec(this->shape[1]);
    for (int j = 0; j < this->shape[1]; j++)
      vec[j] = (*this)(i, j);
    return vec;
  }

  /**
   * @brief Return the array size (number of elements).
   *
   * @return int
   */
  inline int size()
  {
    return this->shape[0] * this->shape[1];
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
   * @brief Set the value of a slice {i1, i2, j1, j2} of data.
   *
   * @param idx Slice extent indices: {i1, i2, j1, j2}.
   * @param value New value.
   */
  inline void set_slice(std::vector<int> idx, float value)
  {
    for (int i = idx[0]; i < idx[1]; i++)
    {
      for (int j = idx[2]; j < idx[3]; j++)
      {
        (*this)(i, j) = value;
      }
    }
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
   * @param hillshading Activate hillshading.
   *
   * **Example**
   * @include ex_perlin.cpp
   */
  void to_png(std::string fname, int cmap, bool hillshading = false);

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
