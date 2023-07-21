#include <algorithm>

#include "highmap/array.hpp"

namespace hmap
{

Array::Array(std::vector<int> shape) : shape(shape)
{
  this->vector.resize(this->shape[0] * this->shape[1]);
}

Array::Array(std::vector<int> shape, float value) : shape(shape)
{
  this->vector.resize(this->shape[0] * this->shape[1]);
  std::fill(this->vector.begin(), this->vector.end(), value);
}

std::vector<int> Array::get_shape()
{
  return shape;
}

std::vector<float> Array::get_vector()
{
  return this->vector;
}

void Array::set_shape(std::vector<int> new_shape)
{
  this->shape = new_shape;
  this->vector.resize(this->shape[0] * this->shape[1]);
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
