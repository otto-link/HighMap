#include <cmath>

#include "highmap/array.hpp"
#include "highmap/op.hpp"

namespace hmap
{

Array convolve1d_i(Array &array, const std::vector<float> &kernel) // private
{
  Array array_out = Array(array.shape);

  // padding extent
  const int nk = (int)kernel.size();
  const int i1 = (int)ceil(0.5f * (float)nk);

  for (int p = 0; p < nk; p++)
  {
    for (int i = 0; i < array.shape[0]; i++)
    {
      int ii;
      // east - west
      if (i + p - i1 < 0)
        ii = -(i + p - i1);
      else if (i + p - i1 > array.shape[0] - 1)
        ii = 2 * array.shape[0] - 1 - (i + p - i1);
      else
        ii = i + p - i1;

      for (int j = 0; j < array.shape[1]; j++)
        array_out(i, j) += array(ii, j) * kernel[p];
    }
  }

  return array_out;
}

Array convolve1d_j(Array &array, const std::vector<float> &kernel) // private
{
  Array array_out = Array(array.shape);

  // padding extent
  const int nk = (int)kernel.size();
  const int j1 = (int)ceil(0.5f * (float)nk);

  for (int q = 0; q < nk; q++)
  {
    for (int j = 0; j < array.shape[1]; j++)
    {
      int jj;

      // north - south
      if (j + q - j1 < 0)
        jj = -(j + q - j1);
      else if (j + q - j1 > array.shape[1] - 1)
        jj = 2 * array.shape[1] - 1 - (j + q - j1);
      else
        jj = j + q - j1;

      for (int i = 0; i < array.shape[0]; i++)
        array_out(i, j) += array(i, jj) * kernel[q];
    }
  }

  return array_out;
}

Array convolve2d(Array &array, Array &kernel)
{
  const int i1 = (int)ceil(0.5f * (float)kernel.shape[0]);
  const int i2 = kernel.shape[0] - i1;
  const int j1 = (int)ceil(0.5f * (float)kernel.shape[1]);
  const int j2 = kernel.shape[1] - j1;

  Array array_buffered = generate_buffered_array(array, {i1, i2, j1, j2});
  Array array_out = convolve2d_truncated(array_buffered, kernel);

  return array_out;
}

Array convolve2d_truncated(Array &array, Array &kernel)
{
  Array array_out = Array(
      {array.shape[0] - kernel.shape[0], array.shape[1] - kernel.shape[1]});

  for (int i = 0; i < array_out.shape[0]; i++)
  {
    for (int j = 0; j < array_out.shape[1]; j++)
    {
      for (int p = 0; p < kernel.shape[0]; p++)
      {
        for (int q = 0; q < kernel.shape[1]; q++)
        {
          array_out(i, j) += array(i + p, j + q) * kernel(p, q);
        }
      }
    }
  }

  return array_out;
}

} // namespace hmap
