#include <vector>

#include "highmap/array.hpp"
#include "highmap/math.hpp"

namespace hmap
{

Array hstack(Array &array1, Array &array2) // friend function
{
  Array array_out = Array({array1.shape[0] + array2.shape[0], array1.shape[1]});

  for (int i = 0; i < array1.shape[0]; i++)
    for (int j = 0; j < array1.shape[1]; j++)
      array_out(i, j) = array1(i, j);

  for (int i = 0; i < array2.shape[0]; i++)
    for (int j = 0; j < array1.shape[1]; j++)
      array_out(i + array1.shape[0], j) = array2(i, j);

  return array_out;
}

Array Array::resample_to_shape(std::vector<int> new_shape)
{
  Array array_out = Array(new_shape);

  // interpolation grid scaled to the starting grid to ease seeking of
  // the reference (i, j) indices during bilinear interpolation
  std::vector<float> x = linspace(0.f, (float)this->shape[0], new_shape[0]);
  std::vector<float> y = linspace(0.f, (float)this->shape[1], new_shape[1]);

  for (int i = 0; i < new_shape[0]; i++)
  {
    int iref = (int)x[i];
    for (int j = 0; j < new_shape[1]; j++)
    {
      int   jref = (int)y[j];
      float u = x[i] - (float)iref;
      float v = y[j] - (float)jref;
      array_out(i, j) = this->get_value_bilinear_at(iref, jref, u, v);
    }
  }

  return array_out;
}

Array vstack(Array &array1, Array &array2) // friend function
{
  Array array_out = Array({array1.shape[0], array1.shape[1] + array2.shape[1]});

  for (int i = 0; i < array1.shape[0]; i++)
  {
    for (int j = 0; j < array1.shape[1]; j++)
      array_out(i, j) = array1(i, j);

    for (int j = 0; j < array2.shape[1]; j++)
      array_out(i, j + array1.shape[0]) = array2(i, j);
  }

  return array_out;
}

} // namespace hmap