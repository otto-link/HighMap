/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file vector.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace hmap
{

/**
 * @brief Vec2 class, for very basic manipulation of 2D vectors.
 *
 * @tparam T Data type.
 */
template <typename T> struct Vec2
{
  T x, y;

  Vec2() : x(0), y(0)
  {
  }

  Vec2(T x, T y) : x(x), y(y)
  {
  }

  bool operator==(const Vec2 &other_vec) const
  {
    return ((this->x == other_vec.x) and (this->y == other_vec.y));
  }

  bool operator!=(const Vec2 &other_vec) const
  {
    return ((this->x != other_vec.x) or (this->y != other_vec.y));
  }

  Vec2 operator/=(const T value)
  {
    this->x /= value;
    this->y /= value;
    return *this;
  }

  Vec2 operator/(const Vec2 &other_vec) const
  {
    Vec2 out;
    out.x = this->x / other_vec.x;
    out.y = this->y / other_vec.y;
    return out;
  }

  Vec2 operator*(const Vec2 &other_vec) const
  {
    Vec2 out;
    out.x = this->x * other_vec.x;
    out.y = this->y * other_vec.y;
    return out;
  }

  Vec2 operator+(const Vec2 &other_vec) const
  {
    Vec2 out;
    out.x = this->x + other_vec.x;
    out.y = this->y + other_vec.y;
    return out;
  }

  Vec2 operator-(const Vec2 &other_vec) const
  {
    Vec2 out;
    out.x = this->x - other_vec.x;
    out.y = this->y - other_vec.y;
    return out;
  }
};

/**
 * @brief Vec3 class, for very basic manipulation of 3D vectors.
 *
 * @tparam T Data type.
 */
template <typename T> struct Vec3
{
  T x, y, z;

  Vec3() : x(0), y(0), z(0)
  {
  }

  Vec3(T x, T y, T z) : x(x), y(y), z(z)
  {
  }

  Vec3 operator/=(const T value)
  {
    this->x /= value;
    this->y /= value;
    this->z /= value;
    return *this;
  }
};

/**
 * @brief Vec4 class, for very basic manipulation of 4D vectors.
 *
 * @tparam T Data type.
 */
template <typename T> struct Vec4
{
  T a, b, c, d;

  Vec4() : a(0), b(0), c(0), d(0)
  {
  }

  Vec4(T a, T b, T c, T d) : a(a), b(b), c(c), d(d)
  {
  }

  Vec4 operator/=(const T value)
  {
    this->a /= value;
    this->b /= value;
    this->c /= value;
    this->d /= value;
    return *this;
  }
};

/**
 * @brief Mat class, for very basic manipulation of matrices.
 *
 * @tparam T Data type.
 */
template <typename T> struct Mat
{
  std::vector<T> vector;
  Vec2<int>      shape;

  Mat(Vec2<int> shape) : shape(shape)
  {
    this->vector.resize(shape.x * shape.y);
  }

  T &operator()(int i, int j)
  {
    return this->vector[i * this->shape.y + j];
  }

  const T &operator()(int i, int j) const
  {
    return this->vector[i * this->shape.y + j];
  }
};

} // namespace hmap
