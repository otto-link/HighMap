/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file algebra.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
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
#include <vector>

namespace hmap
{

/**
 * @brief Vec2 class for basic manipulation of 2D vectors.
 *
 * This class provides basic operations for 2D vectors, such as addition,
 * subtraction, multiplication, division, and dot product calculation.
 * The class supports various data types through templating.
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
   * @brief Parameterized constructor initializing the vector to given values.
   *
   * @param x The x component of the vector.
   * @param y The y component of the vector.
   */
  Vec2(T x, T y) : x(x), y(y)
  {
  }

  /**
   * @brief Equality operator.
   *
   * Compares two vectors for equality.
   *
   * @param other_vec The vector to compare with.
   * @return true if the vectors are equal, false otherwise.
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
   * @param other_vec The vector to compare with.
   * @return true if the vectors are not equal, false otherwise.
   */
  bool operator!=(const Vec2 &other_vec) const
  {
    return ((this->x != other_vec.x) || (this->y != other_vec.y));
  }

  /**
   * @brief Division-assignment operator.
   *
   * Divides both components of the vector by a scalar value and assigns the
   * result.
   *
   * @param value The scalar value to divide by.
   * @return A reference to the current vector after division.
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
   * Divides each component of the vector by the corresponding component of
   * another vector.
   *
   * @param other_vec The vector to divide by.
   * @return A new vector that is the result of the division.
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
   * Multiplies each component of the vector by the corresponding component of
   * another vector.
   *
   * @param other_vec The vector to multiply by.
   * @return A new vector that is the result of the multiplication.
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
   * Adds each component of the vector to the corresponding component of another
   * vector.
   *
   * @param other_vec The vector to add.
   * @return A new vector that is the result of the addition.
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
   * Subtracts each component of another vector from the corresponding component
   * of this vector.
   *
   * @param other_vec The vector to subtract.
   * @return A new vector that is the result of the subtraction.
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
   * @param scalar The scalar value to multiply with.
   * @return Vec2 A new vector with each component multiplied by the scalar.
   */
  Vec2 operator*(T scalar) const
  {
    return Vec2(x * scalar, y * scalar);
  }

  /**
   * @brief Scalar multiplication (scalar * Vec2).
   *
   * Multiplies each component of the vector by a scalar value. This function
   * allows expressions where the scalar is on the left side of the
   * multiplication operator.
   *
   * @param scalar The scalar value to multiply with.
   * @param vec The vector to multiply.
   * @return Vec2 A new vector with each component multiplied by the scalar.
   */
  friend Vec2 operator*(T scalar, const Vec2 &vec)
  {
    return Vec2(scalar * vec.x, scalar * vec.y);
  }

  /**
   * @brief Friend function to calculate the dot product of two vectors.
   *
   * The dot product is the sum of the products of the corresponding components
   * of the vectors.
   *
   * @param v1 The first vector.
   * @param v2 The second vector.
   * @return The dot product of the two vectors.
   */
  friend float dot(const Vec2 v1, const Vec2 v2)
  {
    return v1.x * v2.x + v1.y * v2.y;
  }
};

/**
 * @brief Vec3 class for basic manipulation of 3D vectors.
 *
 * This class provides basic operations for 3D vectors, such as addition,
 * subtraction, multiplication, division, and dot product calculation.
 * The class supports various data types through templating.
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
   * @brief Parameterized constructor initializing the vector to given values.
   *
   * @param x The x component of the vector.
   * @param y The y component of the vector.
   * @param z The z component of the vector.
   */
  Vec3(T x, T y, T z) : x(x), y(y), z(z)
  {
  }

  /**
   * @brief Constructs a Vec3 instance from a vector of components.
   *
   * This constructor initializes the vector using the values stored in a
   * `std::vector<T>`. The vector is expected to have at least three elements
   * where the first element represents the x component, the second element
   * represents the y component, and the third element represents the z
   * component.
   *
   * @param xyz A vector containing the x, y, and z components of the vector.
   *            The size of the vector must be at least 3.
   */
  Vec3(const std::vector<T> &xyz)
  {
    this->x = xyz[0];
    this->y = xyz[1];
    this->z = xyz[2];
  }

  /**
   * @brief Equality operator.
   *
   * Compares two vectors for equality.
   *
   * @param other_vec The vector to compare with.
   * @return true if the vectors are equal, false otherwise.
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
   * @param other_vec The vector to compare with.
   * @return true if the vectors are not equal, false otherwise.
   */
  bool operator!=(const Vec3 &other_vec) const
  {
    return ((this->x != other_vec.x) || (this->y != other_vec.y) ||
            (this->z != other_vec.z));
  }

  /**
   * @brief Division-assignment operator.
   *
   * Divides all components of the vector by a scalar value and assigns the
   * result.
   *
   * @param value The scalar value to divide by.
   * @return A reference to the current vector after division.
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
   * Divides each component of the vector by the corresponding component of
   * another vector.
   *
   * @param other_vec The vector to divide by.
   * @return A new vector that is the result of the division.
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
   * Multiplies each component of the vector by the corresponding component of
   * another vector.
   *
   * @param other_vec The vector to multiply by.
   * @return A new vector that is the result of the multiplication.
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
   * Adds each component of the vector to the corresponding component of another
   * vector.
   *
   * @param other_vec The vector to add.
   * @return A new vector that is the result of the addition.
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
   * Subtracts each component of another vector from the corresponding component
   * of this vector.
   *
   * @param other_vec The vector to subtract.
   * @return A new vector that is the result of the subtraction.
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
   * @param scalar The scalar value to multiply with.
   * @return Vec3 A new vector with each component multiplied by the scalar.
   */
  Vec3 operator*(T scalar) const
  {
    return Vec3(x * scalar, y * scalar, z * scalar);
  }

  /**
   * @brief Scalar multiplication (scalar * Vec3).
   *
   * Multiplies each component of the vector by a scalar value. This function
   * allows expressions where the scalar is on the left side of the
   * multiplication operator.
   *
   * @param scalar The scalar value to multiply with.
   * @param vec The vector to multiply.
   * @return Vec3 A new vector with each component multiplied by the scalar.
   */
  friend Vec3 operator*(T scalar, const Vec3 &vec)
  {
    return Vec3(scalar * vec.x, scalar * vec.y, scalar * vec.z);
  }

  /**
   * @brief Friend function to calculate the dot product of two vectors.
   *
   * The dot product is the sum of the products of the corresponding components
   * of the vectors.
   *
   * @param v1 The first vector.
   * @param v2 The second vector.
   * @return The dot product of the two vectors.
   */
  friend float dot(const Vec3 v1, const Vec3 v2)
  {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  }

  /**
   * @brief Friend function to calculate the cross product of two vectors.
   *
   * The cross product results in a vector that is perpendicular to the plane
   * formed by the two input vectors.
   *
   * @param v1 The first vector.
   * @param v2 The second vector.
   * @return A new vector that is the result of the cross product.
   */
  friend Vec3 cross(const Vec3 v1, const Vec3 v2)
  {
    Vec3 out;
    out.x = v1.y * v2.z - v1.z * v2.y;
    out.y = v1.z * v2.x - v1.x * v2.z;
    out.z = v1.x * v2.y - v1.y * v2.x;
    return out;
  }
};

/**
 * @brief Vec4 class for basic manipulation of 4D vectors.
 *
 * This class provides basic operations for 4D vectors, such as addition,
 * subtraction, multiplication, division, and dot product calculation.
 * The class supports various data types through templating.
 *
 * @tparam T Data type for the vector components (e.g., int, float, double).
 */
template <typename T> struct Vec4
{
  T a, b, c, d; /**< @brief The a, b, c, and d components of the vector. */

  /**
   * @brief Default constructor initializing the vector to (0, 0, 0, 0).
   *
   * Initializes the a, b, c, and d components to zero.
   */
  Vec4() : a(0), b(0), c(0), d(0)
  {
  }

  /**
   * @brief Parameterized constructor initializing the vector to given values.
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
   * @brief Equality operator.
   *
   * Compares two vectors for equality.
   *
   * @param other_vec The vector to compare with.
   * @return true if the vectors are equal, false otherwise.
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
   * @param other_vec The vector to compare with.
   * @return true if the vectors are not equal, false otherwise.
   */
  bool operator!=(const Vec4 &other_vec) const
  {
    return ((this->a != other_vec.a) || (this->b != other_vec.b) ||
            (this->c != other_vec.c) || (this->d != other_vec.d));
  }

  /**
   * @brief Division-assignment operator.
   *
   * Divides all components of the vector by a scalar value and assigns the
   * result.
   *
   * @param value The scalar value to divide by.
   * @return A reference to the current vector after division.
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
   * Divides each component of the vector by the corresponding component of
   * another vector.
   *
   * @param other_vec The vector to divide by.
   * @return A new vector that is the result of the division.
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
   * Multiplies each component of the vector by the corresponding component of
   * another vector.
   *
   * @param other_vec The vector to multiply by.
   * @return A new vector that is the result of the multiplication.
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
   * Adds each component of the vector to the corresponding component of another
   * vector.
   *
   * @param other_vec The vector to add.
   * @return A new vector that is the result of the addition.
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
   * Subtracts each component of another vector from the corresponding component
   * of this vector.
   *
   * @param other_vec The vector to subtract.
   * @return A new vector that is the result of the subtraction.
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
   * @param scalar The scalar value to multiply with.
   * @return Vec4 A new vector with each component multiplied by the scalar.
   */
  Vec4 operator*(T scalar) const
  {
    return Vec4(a * scalar, b * scalar, c * scalar, d * scalar);
  }

  /**
   * @brief Scalar multiplication (scalar * Vec4).
   *
   * Multiplies each component of the vector by a scalar value. This function
   * allows expressions where the scalar is on the left side of the
   * multiplication operator.
   *
   * @param scalar The scalar value to multiply with.
   * @param vec The vector to multiply.
   * @return Vec4 A new vector with each component multiplied by the scalar.
   */
  friend Vec4 operator*(T scalar, const Vec4 &vec)
  {
    return Vec4(scalar * vec.a, scalar * vec.b, scalar * vec.c, scalar * vec.d);
  }

  /**
   * @brief Friend function to calculate the dot product of two vectors.
   *
   * The dot product is the sum of the products of the corresponding components
   * of the vectors.
   *
   * @param v1 The first vector.
   * @param v2 The second vector.
   * @return The dot product of the two vectors.
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
  Vec2<int> shape; /**< @brief Dimensions of the matrix (rows x columns). */

  /**
   * @brief Constructor to initialize a matrix with a given shape.
   *
   * Allocates memory for the matrix elements based on the specified shape.
   * The matrix is initialized with the default value of the type T.
   *
   * @param shape A Vec2<int> representing the number of rows and columns in the
   * matrix.
   */
  Mat(Vec2<int> shape) : shape(shape)
  {
    this->vector.resize(shape.x * shape.y);
  }

  /**
   * @brief Access operator to get a reference to the element at (i, j).
   *
   * Provides non-const access to the matrix element at the specified row and
   * column.
   *
   * @param i Row index (0-based).
   * @param j Column index (0-based).
   * @return A reference to the element at the specified position.
   */
  T &operator()(int i, int j)
  {
    return this->vector[i * this->shape.y + j];
  }

  /**
   * @brief Const access operator to get the value of the element at (i, j).
   *
   * Provides const access to the matrix element at the specified row and
   * column.
   *
   * @param i Row index (0-based).
   * @param j Column index (0-based).
   * @return A const reference to the element at the specified position.
   */
  const T &operator()(int i, int j) const
  {
    return this->vector[i * this->shape.y + j];
  }
};

} // namespace hmap
