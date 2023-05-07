#include "highmap/array.hpp"

Array::Array(std::vector<int> shape) : shape(shape)
{
  this->vector.resize(this->shape[0] * this->shape[1]);
}
