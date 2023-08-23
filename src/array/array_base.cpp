/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <algorithm>
#include <stdexcept>

#include "macrologger.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "highmap/array.hpp"

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

Array::Array(std::string filename)
{
  int width;
  int height;
  int original_no_channels;
  int desired_no_channels = 3;

  unsigned char *img = stbi_load(filename.c_str(),
                                 &width,
                                 &height,
                                 &original_no_channels,
                                 desired_no_channels);

  if (original_no_channels != desired_no_channels)
  {
    LOG_ERROR("original_no_channels: %d (expected to have %d channels)",
              original_no_channels,
              desired_no_channels);
    throw std::runtime_error(
        "png file cannot be loaded, wrong number of channels");
  }

  this->set_shape(Vec2<int>(height, width));

  for (int i = 0; i < this->shape.x; i++)
    for (int j = 0; j < this->shape.y; j++)
    {
      // transpose and flip...
      int k = ((this->shape.y - j - 1) * this->shape.x + i) *
              desired_no_channels;
      (*this)(i, j) = (float)img[k] / 255.f;
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
