/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <stdexcept>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/io.hpp"

namespace hmap
{

Array::Array()
{
}

Array::Array(Vec2<int> shape) : shape(shape)
{
  this->vector.resize(this->shape.x * this->shape.y);
}

Array::Array(Vec2<int> shape, float value) : shape(shape)
{
  this->vector.resize(this->shape.x * this->shape.y);
  std::fill(this->vector.begin(), this->vector.end(), value);
}

Array::Array(std::string filename, bool resize_array)
{
  int      width;
  int      height;
  png_byte color_type;
  png_byte bit_depth;

  read_png_header(filename, width, height, color_type, bit_depth);

  if (resize_array)
    this->set_shape(Vec2<int>(width, height));
  else if (this->shape.x != width || this->shape.y != height)
  {
    // exit if image size does not match current array size
    LOG_ERROR("image size (%d, %d) does not match current size (%d, %d)",
              width,
              height,
              this->shape.x,
              this->shape.y);
    return;
  }

  if (bit_depth == 8)
  {
    LOG_DEBUG("8bit");
    std::vector<uint8_t> img = read_png_grayscale_8bit(filename);

    for (int i = 0; i < this->shape.x; i++)
      for (int j = 0; j < this->shape.y; j++)
      {
        int k = (this->shape.y - 1 - j) * this->shape.y + i;
        (*this)(i, j) = (float)img[k] / 255.f;
      }
  }

  if (bit_depth == 16)
  {
    LOG_DEBUG("16bit");
    std::vector<uint16_t> img = read_png_grayscale_16bit(filename);

    for (int i = 0; i < this->shape.x; i++)
      for (int j = 0; j < this->shape.y; j++)
      {
        int k = (this->shape.y - 1 - j) * this->shape.x + i;
        (*this)(i, j) = (float)img[k] / 65535.f;
      }
  }
}

Vec2<int> Array::get_shape()
{
  return shape;
}

std::vector<float> Array::get_vector()
{
  return this->vector;
}

void Array::set_shape(Vec2<int> new_shape)
{
  this->shape = new_shape;
  this->vector.resize(this->shape.x * this->shape.y);
}

Array Array::operator=(const float value)
{
  std::fill(this->vector.begin(), this->vector.end(), value);
  return *this;
}

Array Array::operator*=(const float value)
{
  std::transform(this->vector.begin(),
                 this->vector.end(),
                 this->vector.begin(),
                 [&value](float v) { return v * value; });
  return *this;
}

Array Array::operator*=(const Array &array)
{
  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array.vector.begin(),
                 this->vector.begin(),
                 [](float v, float a) { return v * a; });
  return *this;
}

Array Array::operator/=(const float value)
{
  std::transform(this->vector.begin(),
                 this->vector.end(),
                 this->vector.begin(),
                 [&value](float v) { return v / value; });
  return *this;
}

Array Array::operator/=(const Array &array)
{
  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array.vector.begin(),
                 this->vector.begin(),
                 [](float v, float a) { return v / a; });
  return *this;
}

Array Array::operator+=(const float value)
{
  std::transform(this->vector.begin(),
                 this->vector.end(),
                 this->vector.begin(),
                 [&value](float v) { return v + value; });
  return *this;
}

Array Array::operator+=(const Array &array)
{
  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array.vector.begin(),
                 this->vector.begin(),
                 [](float v, float a) { return v + a; });
  return *this;
}

Array Array::operator-=(const float value)
{
  std::transform(this->vector.begin(),
                 this->vector.end(),
                 this->vector.begin(),
                 [&value](float v) { return v - value; });
  return *this;
}

Array Array::operator-=(const Array &array)
{
  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array.vector.begin(),
                 this->vector.begin(),
                 [](float v, float a) { return v - a; });
  return *this;
}

Array Array::operator*(const float value) const
{
  Array array_out = Array(this->shape);

  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array_out.vector.begin(),
                 [&value](float v) { return v * value; });
  return array_out;
}

Array Array::operator*(const Array &array) const
{
  Array array_out = Array(array.shape);

  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array.vector.begin(),
                 array_out.vector.begin(),
                 [](float a, float b) { return a * b; });
  return array_out;
}

Array operator*(const float value, const Array &array) // friend function
{
  Array array_out = Array(array.shape);

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [&value](float v) { return v * value; });
  return array_out;
}

Array Array::operator/(const float value) const
{
  Array array_out = Array(this->shape);

  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array_out.vector.begin(),
                 [&value](float v) { return v / value; });
  return array_out;
}

Array Array::operator/(const Array &array) const
{
  Array array_out = Array(array.shape);

  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array.vector.begin(),
                 array_out.vector.begin(),
                 [](float a, float b) { return a / b; });
  return array_out;
}

Array operator/(const float value, const Array &array) // friend function
{
  Array array_out = Array(array.shape);

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [&value](float v) { return value / v; });
  return array_out;
}

Array Array::operator+(const float value) const
{
  Array array_out = Array(this->shape);

  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array_out.vector.begin(),
                 [&value](float v) { return v + value; });
  return array_out;
}

Array Array::operator+(const Array &array) const
{
  Array array_out = Array(array.shape);

  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array.vector.begin(),
                 array_out.vector.begin(),
                 [](float a, float b) { return a + b; });
  return array_out;
}

Array operator+(const float value, const Array &array) // friend function
{
  Array array_out = Array(array.shape);

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [&value](float v) { return value + v; });
  return array_out;
}

Array Array::operator-() const
{
  Array array_out = Array(this->shape);

  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array_out.vector.begin(),
                 [](float v) { return -v; });
  return array_out;
}

Array Array::operator-(float value) const
{
  Array array_out = Array(this->shape);

  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array_out.vector.begin(),
                 [&value](float v) { return v / value; });
  return array_out;
}

Array Array::operator-(const Array &array) const
{
  Array array_out = Array(array.shape);

  std::transform(this->vector.begin(),
                 this->vector.end(),
                 array.vector.begin(),
                 array_out.vector.begin(),
                 [](float a, float b) { return a - b; });
  return array_out;
}

const Array operator-(const float value, const Array &array) // friend function
{
  Array array_out = Array(array.shape);

  std::transform(array.vector.begin(),
                 array.vector.end(),
                 array_out.vector.begin(),
                 [&value](float v) { return value - v; });
  return array_out;
}

} // namespace hmap
