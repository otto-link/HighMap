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

Array generate_buffered_array(Array &array, std::vector<int> buffers)
{
  Array array_out = Array({array.shape[0] + buffers[0] + buffers[1],
                           array.shape[1] + buffers[2] + buffers[3]});

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
      array_out(i + buffers[0], j + buffers[2]) = array(i, j);

  int i1 = buffers[0];
  int i2 = buffers[1];
  int j1 = buffers[2];
  int j2 = buffers[3];

  for (int i = 0; i < i1; i++)
    for (int j = j1; j < array_out.shape[1] - j2; j++)
      array_out(i, j) = array_out(2 * i1 - i, j);

  for (int i = array_out.shape[0] - i2; i < array_out.shape[0]; i++)
    for (int j = j1; j < array_out.shape[1] - j2; j++)
      array_out(i, j) = array_out(2 * (array_out.shape[0] - i2) - i - 1, j);

  for (int i = 0; i < array_out.shape[0]; i++)
    for (int j = 0; j < j1; j++)
      array_out(i, j) = array_out(i, 2 * j1 - j);

  for (int i = 0; i < array_out.shape[0]; i++)
    for (int j = array_out.shape[1] - j2; j < array_out.shape[1]; j++)
      array_out(i, j) = array_out(i, 2 * (array_out.shape[1] - j2) - j - 1);

  return array_out;
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

void sym_borders(Array &array, std::vector<int> buffer_sizes)
{
  const int i1 = buffer_sizes[0];
  const int i2 = buffer_sizes[1];
  const int j1 = buffer_sizes[2];
  const int j2 = buffer_sizes[3];

  // fill-in the blanks...
  for (int i = 0; i < i1; i++)
  {
    for (int j = j1; j < array.shape[1] - j2; j++)
    {
      array(i, j) = array(2 * i1 - i, j);
    }
  }

  for (int i = array.shape[0] - i2; i < array.shape[0]; i++)
  {
    for (int j = j1; j < array.shape[1] - j2; j++)
    {
      array(i, j) = array(2 * (array.shape[0] - i2) - i - 1, j);
    }
  }

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = 0; j < j1; j++)
    {
      array(i, j) = array(i, 2 * j1 - j);
    }
  }

  for (int i = 0; i < array.shape[0]; i++)
  {
    for (int j = array.shape[1] - j2; j < array.shape[1]; j++)
    {
      array(i, j) = array(i, 2 * (array.shape[1] - j2) - j - 1);
    }
  }
}

} // namespace hmap
