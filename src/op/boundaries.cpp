#include <cmath>

#include "highmap/array.hpp"

namespace hmap
{

void extrapolate_borders(Array &array)
{
  const int ni = array.shape[0];
  const int nj = array.shape[1];

  for (int j = 0; j < nj; j++)
  {
    array(0, j) = 2.f * array(1, j) - array(2, j);
    array(ni - 1, j) = 2.f * array(ni - 2, j) - array(ni - 3, j);
  }

  for (int i = 0; i < ni; i++)
  {
    array(i, 0) = 2.f * array(i, 1) - array(i, 2);
    array(i, nj - 1) = 2.f * array(i, nj - 2) - array(i, nj - 3);
  }
}

void fill_borders(Array &array)
{
  const int ni = array.shape[0];
  const int nj = array.shape[1];

  for (int j = 0; j < nj; j++)
  {
    array(0, j) = array(1, j);
    array(ni - 1, j) = array(ni - 2, j);
  }

  for (int i = 0; i < ni; i++)
  {
    array(i, 0) = array(i, 1);
    array(i, nj - 1) = array(i, nj - 2);
  }
}

void set_borders(Array             &array,
                 std::vector<float> border_values,
                 std::vector<int>   buffer_sizes)
{
  // west
  for (int i = 0; i < buffer_sizes[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float r = (float)i / (float)buffer_sizes[0];
      r = r * r * (3.f - 2.f * r);
      array(i, j) = (1.f - r) * border_values[0] + r * array(i, j);
    }

  // east
  for (int i = array.shape[0] - buffer_sizes[1]; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
    {
      float r = 1.f - (float)(i - array.shape[0] + buffer_sizes[1]) /
                          (float)buffer_sizes[1];
      r = r * r * (3.f - 2.f * r);
      array(i, j) = (1.f - r) * border_values[1] + r * array(i, j);
    }

  // south
  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < buffer_sizes[2]; j++)
    {
      float r = (float)j / (float)buffer_sizes[2];
      r = r * r * (3.f - 2.f * r);
      array(i, j) = (1.f - r) * border_values[2] + r * array(i, j);
    }

  // north
  for (int i = 0; i < array.shape[0]; i++)
    for (int j = array.shape[1] - buffer_sizes[3]; j < array.shape[1]; j++)
    {
      float r = 1.f - (float)(j - array.shape[1] + buffer_sizes[3]) /
                          (float)buffer_sizes[3];
      r = r * r * (3.f - 2.f * r);
      array(i, j) = (1.f - r) * border_values[3] + r * array(i, j);
    }
}

void set_borders(Array &array, float border_values, int buffer_sizes)
{
  std::vector<float> bv = {border_values,
                           border_values,
                           border_values,
                           border_values};
  std::vector<int>   bs = {buffer_sizes,
                           buffer_sizes,
                           buffer_sizes,
                           buffer_sizes};
  set_borders(array, bv, bs);
}

} // namespace hmap