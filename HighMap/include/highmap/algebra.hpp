/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file algebra.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief Header file defining basic vector and matrix manipulation classes.
 *
 * This header file provides basic implementations for 2D, 3D, and 4D vector
 * classes (`Vec2`, `Vec3`, `Vec4`) and a simple matrix class (`Mat`). These
 * classes offer fundamental operations such as addition, subtraction,
 * multiplication, division, dot products, and basic matrix manipulation
 * capabilities.
 *
 * @version 0.1
 * @date 2023-08-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <cmath>
#include <stdexcept>
#include <vector>

namespace hmap
{

/**
 * @brief Vec2 class for basic manipulation of 2D vectors.
 *
 * This class provides basic operations for 2D vectors, such as addition,
 * subtraction, multiplication, division, and dot product calculation. The class
 * supports various data types through templating.
 *
 * @tparam T Data type for the vector components (e.g., int, float, double).
 */
template <typename T> struct Vec2
{
  T x, y; /**< @brief The x and y components of the vector. */

  /**
   * @brief Default constructor initializing the vector to (0, 0).
   *
   * Initializes both x and y components to zero.
   */
  Vec2() : x(0), y(0)
  {
  }

  /**
   * @brief Parameterized constructor initializing the vector to given
   * values.
   *
   * @param x The x component of the vector.
   * @param y The y component of the vector.
   */
  Vec2(T x, T y) : x(x), y(y)
  {
  }

  /**
   * @brief Constructs a Vec2 object from a std::vector.
   *
   * This constructor takes a vector containing exactly two elements and
   * assigns the first element to `x` and the second element to `y`.
   *
   * @tparam T The type of elements in the vector (e.g., float, int,
   * double).
   * @param vec A const reference to a vector of size 2, where the first
   *            element corresponds to `x` and the second to `y`.
   *
   * @throw std::invalid_argument If the vector does not contain exactly
   * two elements.
   */
  Vec2(const std::vector<T> &vec) : x(0), y(0)
  {
    if (vec.size() != 2)
    {
      throw std::invalid_argument("Vector must contain exactly two elements.");
    }
    x = vec[0];
    y = vec[1];
  }

  /**
   * @brief Equality operator.
   *
   * Compares two vectors for equality.
   *
   * @param  other_vec The vector to compare with.
   * @return           true if the vectors are equal, false otherwise.
   */
  bool operator==(const Vec2 &other_vec) const
  {
    return ((this->x == other_vec.x) && (this->y == other_vec.y));
  }

  /**
   * @brief Inequality operator.
   *
   * Compares two vectors for inequality.
   *
   * @param  other_vec The vector to compare with.
   * @return           true if the vectors are not equal, false otherwise.
   */
  bool operator!=(const Vec2 &other_vec) const
  {
    return ((this->x != other_vec.x) || (this->y != other_vec.y));
  }

  /**
   * @brief Division-assignment operator.
   *
   * Divides both components of the vector by a scalar value and assigns
   * the result.
   *
   * @param  value The scalar value to divide by.
   * @return       A reference to the current vector after division.
   */
  Vec2 &operator/=(const T value)
  {
    this->x /= value;
    this->y /= value;
    return *this;
  }

  /**
   * @brief Division operator.
   *
   * Divides each component of the vector by the corresponding component
   * of another vector.
   *
   * @param  other_vec The vector to divide by.
   * @return           A new vector that is the result of the division.
   */
  Vec2 operator/(const Vec2 &other_vec) const
  {
    Vec2 out;
    out.x = this->x / other_vec.x;
    out.y = this->y / other_vec.y;
    return out;
  }

  /**
   * @brief Multiplication operator.
   *
   * Multiplies each component of the vector by the corresponding
   * component of another vector.
   *
   * @param  other_vec The vector to multiply by.
   * @return           A new vector that is the result of the
   *                   multiplication.
   */
  Vec2 operator*(const Vec2 &other_vec) const
  {
    Vec2 out;
    out.x = this->x * other_vec.x;
    out.y = this->y * other_vec.y;
    return out;
  }

  /**
   * @brief Addition operator.
   *
   * Adds each component of the vector to the corresponding component of
   * another vector.
   *
   * @param  other_vec The vector to add.
   * @return           A new vector that is the result of the addition.
   */
  Vec2 operator+(const Vec2 &other_vec) const
  {
    Vec2 out;
    out.x = this->x + other_vec.x;
    out.y = this->y + other_vec.y;
    return out;
  }

  /**
   * @brief Subtraction operator.
   *
   * Subtracts each component of another vector from the corresponding
   * component of this vector.
   *
   * @param  other_vec The vector to subtract.
   * @return           A new vector that is the result of the subtraction.
   */
  Vec2 operator-(const Vec2 &other_vec) const
  {
    Vec2 out;
    out.x = this->x - other_vec.x;
    out.y = this->y - other_vec.y;
    return out;
  }

  /**
   * @brief Scalar multiplication (Vec2 * scalar).
   *
   * Multiplies each component of the vector by a scalar value.
   *
   * @param  scalar The scalar value to multiply with.
   * @return        Vec2 A new vector with each component multiplied by
   *                the scalar.
   */
  Vec2 operator*(T scalar) const
  {
    return Vec2(x * scalar, y * scalar);
  }

  /**
   * @brief Scalar multiplication (scalar * Vec2).
   *
   * Multiplies each component of the vector by a scalar value. This
   * function allows expressions where the scalar is on the left side of
   * the multiplication operator.
   *
   * @param  scalar The scalar value to multiply with.
   * @param  vec    The vector to multiply.
   * @return        Vec2 A new vector with each component multiplied by
   *                the scalar.
   */
  friend Vec2 operator*(T scalar, const Vec2 &vec)
  {
    return Vec2(scalar * vec.x, scalar * vec.y);
  }

  /**
   * @brief Friend function to calculate the dot product of two vectors.
   *
   * The dot product is the sum of the products of the corresponding
   * components of the vectors.
   *
   * @param  v1 The first vector.
   * @param  v2 The second vector.
   * @return    The dot product of the two vectors.
   */
  friend float dot(const Vec2 v1, const Vec2 v2)
  {
    return v1.x * v2.x + v1.y * v2.y;
  }

  /**
   * @brief Calculate the magnitude (length) of the vector.
   *
   * @return The magnitude of the vector.
   */
  T magnitude() const
  {
    return std::sqrt(x * x + y * y);
  }

  /**
   * @brief Normalize the vector to have a magnitude of 1.
   *
   * This method modifies the vector in place. If the vector has zero
   * length, the components remain unchanged to avoid division by zero.
   */
  void normalize()
  {
    T mag = magnitude();
    if (mag > 0) // Avoid division by zero
    {
      x /= mag;
      y /= mag;
    }
  }
};

/**
 * @brief Vec3 class for basic manipulation of 3D vectors.
 *
 * This class provides basic operations for 3D vectors, such as addition,
 * subtraction, multiplication, division, and dot product calculation. The class
 * supports various data types through templating.
 *
 * @tparam T Data type for the vector components (e.g., int, float, double).
 */
template <typename T> struct Vec3
{
  T x, y, z; /**< @brief The x, y, and z components of the vector. */

  /**
   * @brief Default constructor initializing the vector to (0, 0, 0).
   *
   * Initializes the x, y, and z components to zero.
   */
  Vec3() : x(0), y(0), z(0)
  {
  }

  /**
   * @brief Parameterized constructor initializing the vector to given
   * values.
   *
   * @param x The x component of the vector.
   * @param y The y component of the vector.
   * @param z The z component of the vector.
   */
  Vec3(T x, T y, T z) : x(x), y(y), z(z)
  {
  }

  /**
   * @brief Constructs a Vec3 object from a std::vector.
   *
   * This constructor takes a vector containing exactly three elements and
   * assigns the first element to `x`, the second element to `y`, and the
   * third to `z`.
   *
   * @tparam T The type of elements in the vector (e.g., float, int,
   * double).
   * @param vec A const reference to a vector of size 3, where the first
   *            element corresponds to `x`, the second to `y`, and the
   *            third to `z`.
   *
   * @throw std::invalid_argument If the vector does not contain exactly
   * three elements.
   */
  Vec3(const std::vector<T> &vec) : x(0), y(0), z(0)
  {
    if (vec.size() != 3)
    {
      throw std::invalid_argument(
          "Vector must contain exactly three elements.");
    }
    x = vec[0];
    y = vec[1];
    z = vec[2];
  }

  /**
   * @brief Equality operator.
   *
   * Compares two vectors for equality.
   *
   * @param  other_vec The vector to compare with.
   * @return           true if the vectors are equal, false otherwise.
   */
  bool operator==(const Vec3 &other_vec) const
  {
    return ((this->x == other_vec.x) && (this->y == other_vec.y) &&
            (this->z == other_vec.z));
  }

  /**
   * @brief Inequality operator.
   *
   * Compares two vectors for inequality.
   *
   * @param  other_vec The vector to compare with.
   * @return           true if the vectors are not equal, false otherwise.
   */
  bool operator!=(const Vec3 &other_vec) const
  {
    return ((this->x != other_vec.x) || (this->y != other_vec.y) ||
            (this->z != other_vec.z));
  }

  /**
   * @brief Division-assignment operator.
   *
   * Divides all components of the vector by a scalar value and assigns
   * the result.
   *
   * @param  value The scalar value to divide by.
   * @return       A reference to the current vector after division.
   */
  Vec3 &operator/=(const T value)
  {
    this->x /= value;
    this->y /= value;
    this->z /= value;
    return *this;
  }

  /**
   * @brief Division operator.
   *
   * Divides each component of the vector by the corresponding component
   * of another vector.
   *
   * @param  other_vec The vector to divide by.
   * @return           A new vector that is the result of the division.
   */
  Vec3 operator/(const Vec3 &other_vec) const
  {
    Vec3 out;
    out.x = this->x / other_vec.x;
    out.y = this->y / other_vec.y;
    out.z = this->z / other_vec.z;
    return out;
  }

  /**
   * @brief Multiplication operator.
   *
   * Multiplies each component of the vector by the corresponding
   * component of another vector.
   *
   * @param  other_vec The vector to multiply by.
   * @return           A new vector that is the result of the
   *                   multiplication.
   */
  Vec3 operator*(const Vec3 &other_vec) const
  {
    Vec3 out;
    out.x = this->x * other_vec.x;
    out.y = this->y * other_vec.y;
    out.z = this->z * other_vec.z;
    return out;
  }

  /**
   * @brief Addition operator.
   *
   * Adds each component of the vector to the corresponding component of
   * another vector.
   *
   * @param  other_vec The vector to add.
   * @return           A new vector that is the result of the addition.
   */
  Vec3 operator+(const Vec3 &other_vec) const
  {
    Vec3 out;
    out.x = this->x + other_vec.x;
    out.y = this->y + other_vec.y;
    out.z = this->z + other_vec.z;
    return out;
  }

  /**
   * @brief Subtraction operator.
   *
   * Subtracts each component of another vector from the corresponding
   * component of this vector.
   *
   * @param  other_vec The vector to subtract.
   * @return           A new vector that is the result of the subtraction.
   */
  Vec3 operator-(const Vec3 &other_vec) const
  {
    Vec3 out;
    out.x = this->x - other_vec.x;
    out.y = this->y - other_vec.y;
    out.z = this->z - other_vec.z;
    return out;
  }

  /**
   * @brief Scalar multiplication (Vec3 * scalar).
   *
   * Multiplies each component of the vector by a scalar value.
   *
   * @param  scalar The scalar value to multiply with.
   * @return        Vec3 A new vector with each component multiplied by
   *                the scalar.
   */
  Vec3 operator*(T scalar) const
  {
    return Vec3(x * scalar, y * scalar, z * scalar);
  }

  /**
   * @brief Scalar multiplication (scalar * Vec3).
   *
   * Multiplies each component of the vector by a scalar value. This
   * function allows expressions where the scalar is on the left side of
   * the multiplication operator.
   *
   * @param  scalar The scalar value to multiply with.
   * @param  vec    The vector to multiply.
   * @return        Vec3 A new vector with each component multiplied by
   *                the scalar.
   */
  friend Vec3 operator*(T scalar, const Vec3 &vec)
  {
    return Vec3(scalar * vec.x, scalar * vec.y, scalar * vec.z);
  }

  /**
   * @brief Friend function to calculate the cross product of two vectors.
   *
   * The cross product results in a vector that is perpendicular to the
   * plane formed by the two input vectors.
   *
   * @param  v1 The first vector.
   * @param  v2 The second vector.
   * @return    A new vector that is the result of the cross product.
   */
  friend Vec3 cross(const Vec3 v1, const Vec3 v2)
  {
    Vec3 out;
    out.x = v1.y * v2.z - v1.z * v2.y;
    out.y = v1.z * v2.x - v1.x * v2.z;
    out.z = v1.x * v2.y - v1.y * v2.x;
    return out;
  }

  /**
   * @brief Friend function to calculate the dot product of two vectors.
   *
   * The dot product is the sum of the products of the corresponding
   * components of the vectors.
   *
   * @param  v1 The first vector.
   * @param  v2 The second vector.
   * @return    The dot product of the two vectors.
   */
  friend float dot(const Vec3 v1, const Vec3 v2)
  {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  }

  /**
   * @brief Calculate the magnitude (length) of the vector.
   *
   * @return The magnitude of the vector.
   */
  T magnitude() const
  {
    return std::sqrt(x * x + y * y + z * z);
  }

  /**
   * @brief Normalize the vector to have a magnitude of 1.
   *
   * This method modifies the vector in place. If the vector has zero
   * length, the components remain unchanged to avoid division by zero.
   */
  void normalize()
  {
    T mag = magnitude();
    if (mag > 0) // Avoid division by zero
    {
      x /= mag;
      y /= mag;
      z /= mag;
    }
  }

  /**
   * @brief Calculate the sum of the vector components.
   *
   * @return The sum of the vector components.
   */
  T sum() const
  {
    return x + y + z;
  }
};

/**
 * @brief Vec4 class for basic manipulation of 4D vectors.
 *
 * This class provides basic operations for 4D vectors, such as addition,
 * subtraction, multiplication, division, and dot product calculation. The class
 * supports various data types through templating.
 *
 * @tparam T Data type for the vector components (e.g., int, float, double).
 */
template <typename T> struct Vec4
{
  T a, b, c, d; /**< @brief The a, b, c, and d components of the vector.
                 */

  /**
   * @brief Default constructor initializing the vector to (0, 0, 0, 0).
   *
   * Initializes the a, b, c, and d components to zero.
   */
  Vec4() : a(0), b(0), c(0), d(0)
  {
  }

  /**
   * @brief Parameterized constructor initializing the vector to given
   * values.
   *
   * @param a The a component of the vector.
   * @param b The b component of the vector.
   * @param c The c component of the vector.
   * @param d The d component of the vector.
   */
  Vec4(T a, T b, T c, T d) : a(a), b(b), c(c), d(d)
  {
  }

  /**
   * @brief Constructs a Vec4 object from a std::vector.
   *
   * This constructor takes a vector containing exactly four elements and
   * assigns the first element to `a`, the second to `b`, the third to
   * `c`, and the fourth to `d`.
   *
   * @tparam T The type of elements in the vector (e.g., float, int,
   * double).
   * @param vec A const reference to a vector of size 4, where the first
   *            element corresponds to `a`, the second to `b`, the third
   *            to `c`, and the fourth to
   * `d`.
   *
   * @throw std::invalid_argument If the vector does not contain exactly
   * four elements.
   */
  Vec4(const std::vector<T> &vec) : a(0), b(0), c(0), d(0)
  {
    if (vec.size() != 4)
    {
      throw std::invalid_argument("Vector must contain exactly four elements.");
    }
    a = vec[0];
    b = vec[1];
    c = vec[2];
    d = vec[3];
  }

  /**
   * @brief Equality operator.
   *
   * Compares two vectors for equality.
   *
   * @param  other_vec The vector to compare with.
   * @return           true if the vectors are equal, false otherwise.
   */
  bool operator==(const Vec4 &other_vec) const
  {
    return ((this->a == other_vec.a) && (this->b == other_vec.b) &&
            (this->c == other_vec.c) && (this->d == other_vec.d));
  }

  /**
   * @brief Inequality operator.
   *
   * Compares two vectors for inequality.
   *
   * @param  other_vec The vector to compare with.
   * @return           true if the vectors are not equal, false otherwise.
   */
  bool operator!=(const Vec4 &other_vec) const
  {
    return ((this->a != other_vec.a) || (this->b != other_vec.b) ||
            (this->c != other_vec.c) || (this->d != other_vec.d));
  }

  /**
   * @brief Division-assignment operator.
   *
   * Divides all components of the vector by a scalar value and assigns
   * the result.
   *
   * @param  value The scalar value to divide by.
   * @return       A reference to the current vector after division.
   */
  Vec4 &operator/=(const T value)
  {
    this->a /= value;
    this->b /= value;
    this->c /= value;
    this->d /= value;
    return *this;
  }

  /**
   * @brief Division operator.
   *
   * Divides each component of the vector by the corresponding component
   * of another vector.
   *
   * @param  other_vec The vector to divide by.
   * @return           A new vector that is the result of the division.
   */
  Vec4 operator/(const Vec4 &other_vec) const
  {
    Vec4 out;
    out.a = this->a / other_vec.a;
    out.b = this->b / other_vec.b;
    out.c = this->c / other_vec.c;
    out.d = this->d / other_vec.d;
    return out;
  }

  /**
   * @brief Multiplication operator.
   *
   * Multiplies each component of the vector by the corresponding
   * component of another vector.
   *
   * @param  other_vec The vector to multiply by.
   * @return           A new vector that is the result of the
   *                   multiplication.
   */
  Vec4 operator*(const Vec4 &other_vec) const
  {
    Vec4 out;
    out.a = this->a * other_vec.a;
    out.b = this->b * other_vec.b;
    out.c = this->c * other_vec.c;
    out.d = this->d * other_vec.d;
    return out;
  }

  /**
   * @brief Addition operator.
   *
   * Adds each component of the vector to the corresponding component of
   * another vector.
   *
   * @param  other_vec The vector to add.
   * @return           A new vector that is the result of the addition.
   */
  Vec4 operator+(const Vec4 &other_vec) const
  {
    Vec4 out;
    out.a = this->a + other_vec.a;
    out.b = this->b + other_vec.b;
    out.c = this->c + other_vec.c;
    out.d = this->d + other_vec.d;
    return out;
  }

  /**
   * @brief Subtraction operator.
   *
   * Subtracts each component of another vector from the corresponding
   * component of this vector.
   *
   * @param  other_vec The vector to subtract.
   * @return           A new vector that is the result of the subtraction.
   */
  Vec4 operator-(const Vec4 &other_vec) const
  {
    Vec4 out;
    out.a = this->a - other_vec.a;
    out.b = this->b - other_vec.b;
    out.c = this->c - other_vec.c;
    out.d = this->d - other_vec.d;
    return out;
  }

  /**
   * @brief Scalar multiplication (Vec4 * scalar).
   *
   * Multiplies each component of the vector by a scalar value.
   *
   * @param  scalar The scalar value to multiply with.
   * @return        Vec4 A new vector with each component multiplied by
   *                the scalar.
   */
  Vec4 operator*(T scalar) const
  {
    return Vec4(a * scalar, b * scalar, c * scalar, d * scalar);
  }

  /**
   * @brief Scalar multiplication (scalar * Vec4).
   *
   * Multiplies each component of the vector by a scalar value. This
   * function allows expressions where the scalar is on the left side of
   * the multiplication operator.
   *
   * @param  scalar The scalar value to multiply with.
   * @param  vec    The vector to multiply.
   * @return        Vec4 A new vector with each component multiplied by
   *                the scalar.
   */
  friend Vec4 operator*(T scalar, const Vec4 &vec)
  {
    return Vec4(scalar * vec.a, scalar * vec.b, scalar * vec.c, scalar * vec.d);
  }

  /**
   * @brief Adjusts the components of the vector by the given offsets.
   *
   * This method creates a new vector by adding the specified offsets to
   * the respective components of the current vector.
   *
   * @tparam T The type of the vector components.
   * @param  da The offset to add to the first component (`a`).
   * @param  db The offset to add to the second component (`b`).
   * @param  dc The offset to add to the third component (`c`).
   * @param  dd The offset to add to the fourth component (`d`).
   * @return    Vec4<T> A new vector with adjusted components.
   */
  Vec4<T> adjust(float da, float db, float dc, float dd)
  {
    return Vec4<T>(this->a + da, this->b + db, this->c + dc, this->d + dd);
  }

  /**
   * @brief Friend function to calculate the dot product of two vectors.
   *
   * The dot product is the sum of the products of the corresponding
   * components of the vectors.
   *
   * @param  v1 The first vector.
   * @param  v2 The second vector.
   * @return    The dot product of the two vectors.
   */
  friend float dot(const Vec4 v1, const Vec4 v2)
  {
    return v1.a * v2.a + v1.b * v2.b + v1.c * v2.c + v1.d * v2.d;
  }
};

/**
 * @brief Mat class for basic manipulation of 2D matrices.
 *
 * This class provides basic operations for 2D matrices, such as element access
 * and initialization. It stores the matrix elements in a 1D vector and provides
 * a convenient interface for accessing elements using 2D indices.
 *
 * @tparam T Data type for the matrix elements (e.g., int, float, double).
 */
template <typename T> struct Mat
{
  std::vector<T> vector; /**< @brief 1D vector storing matrix elements in
                            row-major order. */
  Vec2<int> shape;       /**< @brief Dimensions of the matrix (rows x columns).
                          */

  /**
   * @brief Constructor to initialize a matrix with a given shape.
   *
   * Allocates memory for the matrix elements based on the specified
   * shape. The matrix is initialized with the default value of the type
   * T.
   *
   * @param shape A Vec2<int> representing the number of rows and columns
   *              in the matrix.
   */
  Mat(Vec2<int> shape) : shape(shape)
  {
    this->vector.resize(shape.x * shape.y);
  }

  /**
   * @brief Access operator to get a reference to the element at (i, j).
   *
   * Provides non-const access to the matrix element at the specified row
   * and column.
   *
   * @param  i Row index (0-based).
   * @param  j Column index (0-based).
   * @return   A reference to the element at the specified position.
   */
  T &operator()(int i, int j)
  {
    return this->vector[i * this->shape.y + j];
  }

  /**
   * @brief Const access operator to get the value of the element at (i,
   * j).
   *
   * Provides const access to the matrix element at the specified row and
   * column.
   *
   * @param  i Row index (0-based).
   * @param  j Column index (0-based).
   * @return   A const reference to the element at the specified position.
   */
  const T &operator()(int i, int j) const
  {
    return this->vector[i * this->shape.y + j];
  }
};

/**
 * @brief Constructs a normalized 3D vector.
 *
 * This function takes three components of a vector (x, y, z), creates a Vec3<T>
 * object, normalizes it, and returns the normalized vector.
 *
 * @tparam T The data type of the vector components (e.g., float, double, etc.).
 * @param  x The x-component of the vector.
 * @param  y The y-component of the vector.
 * @param  z The z-component of the vector.
 * @return   Vec3<T> A normalized 3D vector of type T.
 */
template <typename T> Vec3<T> normalized_vec3(T x, T y, T z)
{
  Vec3<T> v = Vec3<T>(x, y, z);
  v.normalize();
  return v;
}

} // namespace hmap
