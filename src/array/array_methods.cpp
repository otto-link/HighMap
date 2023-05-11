#include "highmap/array.hpp"

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